<?php
// $Id$

/**
 * @file
 */
 
/**
 * Theme function for 'default' ogdi_field field formatter.
 * 
 * $element['#item']: the sanitized $delta value for the item,
 * $element['#field_name']: the field name,
 * $element['#type_name']: the $node->type,
 * $element['#formatter']: the $formatter_name,
 * $element['#node']: the $node,
 * $element['#delta']: the delta of this item, like '0',
 * 
 */
function theme_ogdi_field_formatter_default($variables) {
  $output = '';
  // If there is a dataset to load 
  if (!empty($variables['display_url'])) {
    if($variables['delta'] > 0) { 
      /* Add the necessary javascript and css files...*/
      /* Jquery UI */
      drupal_add_library('system', 'ui.tabs');
      drupal_add_library('system', 'ui.dialog');
  
      /* DataTables CSS and JS */
      drupal_add_js(drupal_get_path('module', 'ogdi_field') .'/plugins/dataTables/media/js/jquery.dataTables.min.js', array('type' => 'file', 'group' => JS_LIBRARY));
      drupal_add_css(drupal_get_path('module', 'ogdi_field') .'/plugins/dataTables/media/css/demo_table.css', array('type' => 'file', 'media' => 'all'));
      drupal_add_css(drupal_get_path('module', 'ogdi_field') .'/plugins/dataTables/media/css/demo_table_jui.css', array('type' => 'file', 'media' => 'all', 'group' => JS_LIBRARY));
    
      /* Add bing maps stuff */
      drupal_add_js('http://ecn.dev.virtualearth.net/mapcontrol/mapcontrol.ashx?v=7.0', array('type' => 'external', 'group' => JS_LIBRARY, 'scope' => 'header'));
      drupal_add_js('http://jquery-bing-maps.googlecode.com/svn/trunk/js/min/jquery.ui.bmap.min.js', array('type' => 'external', 'group' => JS_LIBRARY, 'scope' => 'header'));
      
      /* OGDI Stuff */
      drupal_add_js(drupal_get_path('module', 'ogdi_field') .'/js/ogdi_field.js', array('type' => 'file', 'scope' => 'footer'));
      drupal_add_css(drupal_get_path('module', 'ogdi_field') ."/css/ogdi_field.css", array('type' => 'file', 'media' => 'all'));
    }
    
    // Set the field markers as setting variables so that we can retrieve what fields we want to add to the markers...
    drupal_add_js(array(
      'ogdi_field' => array(
        $variables['delta'] => array(
          'title' => $variables['markers']['title'],
          'description' => $variables['markers']['description'],
          'latitude' => $variables['markers']['latitude'],
          'longitude' => $variables['markers']['longitude']
          ),
        )
      ), 'setting'
    );
  
    /* Reset the sticky table headers - see theme_ogdi_field_container() */
    //drupal_add_js('$("table.no-sticky thead").addClass("tableHeader-processed");', array('type' => 'inline', 'scope' => 'header'));

    $parameters = array(
      'format' => 'json',
    );
    
    // Get information about the dataset...
    $datainfo = ogdi_field_get_data_info($variables['display_url']);
    
    // Create a new instance of OGDI object...
    $ogdi = ogdi_field_ogdi($datainfo['datasource'], $datainfo['dataset']);
    
    // Get the schema (column names) of the dataset...
    $schema = ogdi_field_get_metadata($ogdi);
    
    // Build the table headers from that...
    $headers = ogdi_field_build_table_headers($schema->d[0]);
    
    
    // Get the data...
    $data = ogdi_field_get_data($ogdi, $parameters);
    
    // Build the table rows from that data...
    $rows = ogdi_field_build_table($data->d, $headers, $variables['delta']);
        
    // Build the OGDI data form...
    $form_variables = array('ogdi' => $ogdi, 'headers' => $headers, 'rows' => $rows, 'parameters' => $parameters, 'delta' => $variables['delta']);
    $form = drupal_get_form('ogdi_field_data_form', $form_variables);
    $output .= drupal_render($form);
  } 
  return $output;
}


/**
 * Theme function for 'plain' ogdi_field field formatter.
 */
function theme_ogdi_field_formatter_plain($variables) {
  return strip_tags($variables['display_url']);
}

/**
 * Theme function for 'link' ogdi_field field formatter.
 */
function theme_ogdi_field_formatter_link($variables) {
  return l(strip_tags($variables['display_url']), strip_tags($variables['display_url']));
}


/**
 * Theme function for OGDI data grid and Map in tabs
 */
function theme_ogdi_field_container($variables) {
  $delta = $variables['delta'];
  $output = '';
  $output .= '<div class="ogdi-wrapper">' . chr(10); 
    $output .= '<div class="ogdi-tabs">'. chr(10);
      // Build the tabs...
      $output .= '<ul>'. chr(10);
      $output .= '<li><a href="#ogdi-data-'. $delta .'">Data</a></li>' . chr(10);;
      $output .= '<li><a href="#ogdi-map-'. $delta .'">Map</a></li>' . chr(10);;
      $output .= '</ul>'. chr(10);
      // Output the data table...
      $output .= '<div id="ogdi-data-'. $delta .'">' . chr(10);
      $output .= theme('ogdi_field_datatable', array('headers' => $variables['headers'], 'rows' => $variables['rows'], 'datasource' => $variables['ogdi']->datasource, 'dataset_id' => $variables['ogdi']->dataset_id, 'parameters' => $variables['parameters']));
      $output .= '</div>' . chr(10);
      // Output the map...
      $output .= '<div id="ogdi-map-'. $delta .'" class="ogdi-map">' . chr(10);
      $output .= theme('ogdi_field_map', array('datasource' => $variables['ogdi']->datasource, 'dataset_id' => $variables['ogdi']->dataset_id, 'parameters' => $variables['parameters'], 'delta' => $delta));
      $output .= '</div>' . chr(10);
    $output .= '</div>' . chr(10);
  $output .= '</div>' . chr(10);
  return $output;
}


/**
 * Theme function for the OGDI data grid
 */
function theme_ogdi_field_datatable($variables) {
  foreach ($variables['parameters'] as $key => $value) {
    if ($value == 'json') {
      continue;
    }
    $querystring[$key] = $value;
  }

  $output = '';
  //kpr($variables);
  // Output the Path to the ATOM Feed API
  $output .= theme('ogdi_field_atom_link', array('datasource' => $variables['datasource'], 'dataset_id' => $variables['dataset_id'], 'querystring' => $querystring));

  // Output the Download select box
 // $output .= theme('ogdi_field_download_link', $datasource, $dataset_id, $querystring);
  
  // If no data is returned...
  if (count($variables['rows']) == 0) {
    $output = '<div class="ogdi-datagrid">' . t('No data could be found, please try again') . '</div>';
  } 
  else {      
    $output .= theme('table', array(
      'header' => $variables['headers'],
      'rows' => $variables['rows'],
      'attributes' => array('class' => array('no-sticky', 'ogdi-datagrid'))
      )
    );
  }
  return $output;
}


/**
 * Theme function for the OGDI Map
 */
function theme_ogdi_field_map($variables) {
  $variables['parameters']['format'] = 'json';
  $delta = $variables['delta'];
  foreach ($variables['parameters'] as $key => $value) {
      $string[] = $key .'='. urlencode($value);
    }
  $querystring = implode('&', $string);
  
  $output = '';
  // Output the Path to the ATOM Feed API
  // $output .= theme('ogdi_field_atom_link', $datasource, $dataset_id, $querystring);

  $output .= '<div class="bing-map" id="ogdi-map-'. $variables['delta'] . '">';
  $output .= $variables['datasource'] . '/' . $variables['dataset_id'] .'?'. $querystring .'&callback=?';
  $output .= '</div>';
  return $output;
}


/**
 * Theme function for the OGDI Filter helptext link and modal window
 */
function theme_ogdi_field_helptext($field) {
  $output = '';
  // If there is a value in the field, we want to ouput it..
  if (!empty($field['']['#value'])) {
    //drupal_add_library('system', 'ui.dialog'); // Load the dialog widget...
    $current_page = drupal_get_path_alias($_GET['q']);
    $output .= l('Help on how to write expressions', $current_page, $options = array('attributes' => array('class' => array('ogdi-helptip')), 'html' => TRUE));  // Output the help icon...
    $output .= '<div class="ogdi-helptext" title="' . t('Expression filter hints') . '">' . chr(10);
    $output .= $field['']['#value'] . chr(10);
    $output .= '</div>' . chr(10) ;
  }
  return $output;
}


/**
 * Theme function to output the Path to the ATOM Feed API of the visible data
 */
function theme_ogdi_field_atom_link($variables) {
  // Output the Path to the ATOM Feed API
  //kpr($variables);
  $url = url($variables['datasource'] . '/' . $variables['dataset_id'], $options = array('html' => TRUE, 'query' => $querystring));
  $output .= '<p>' . urldecode(t('Full query URL: !url', array('!url' => l($url, $url, $options = array('html' => TRUE)))));
  $output .= t(' (Click to view results as XML/Atom)') . '</p>';

  return $output;
}


/**
 * Theme function to output the select field for choosing the type of file format to download the visible data
 */
function theme_ogdi_field_download_link($datasource, $dataset_id, $querystring) {
  
  $output .= l('Download', '');
  $form['download']['filetypes'] = array(
    '#type' => 'value',
    '#value' => array(t('CSV'), t('Excel'), t('Daisy'), t('KML')),
  );
  
  $form['download']['choose'] = array(
    '#id' => t('ogdi-choosetype'),
    '#type' => 'select',
    '#options' => $form['download']['filetypes']['#value'],
    '#name' => t('ogdi-choosetype')
  );
  $output .= drupal_render($form['download']);
  return $output;
}


/**
 * Function to build the OGDI filter form that will run queries on the page
 */
function ogdi_field_filter_form($form, &$form_state, $ogdi, $element) {
  // Get the current field so that we can set the titles and descriptions on form elements...
  //$field = content_fields($element['#field_name']);
  $form['query'] = array(
    '#title' => t('Filter the dataset'),
    '#type' => 'fieldset',
    '#collapsed' => TRUE,
    '#collapsible' => TRUE
  );  
  
  $form['query']['filter'] = array(
    '#type'         =>  'textarea',
    '#title'        =>  'Filter',//$field['filter_label'],
    '#description'  =>  'Description',//$field['filter_description'],
    //'#element_validate' =>  array('ogdi_field_filter_validate')
  );
  
  $form['query']['helptext'] = array(
    '#value'        =>  'Helptext',//$field['filter_helptext'],
    '#theme'        =>  'ogdi_field_helptext'
  );
    
  $form['query']['datasource'] = array(
    '#type'   => 'hidden',
    '#value'  => $ogdi->datasource
  );
  
  $form['query']['dataset_id'] = array(
    '#type'   => 'hidden',
    '#value'  => $ogdi->dataset_id
  );
  
  // On submit we want to run an ajax callback...
  $form['query']['submit'] = array(
    '#type'   =>  'submit',
    '#value'  =>  t('Run'),
    '#ajax' => array(
      'callback' => 'ogdi_field_filter_callback',  // See hook_menu for this...
      'wrapper' => 'ogdi-wrapper',
      'progress' => array('type' => 'throbber', 'message' => t('Please wait...')),
      'method' =>  'replace',
      'effect' =>  'fade'
     ), 
  );
  return $form;
}