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
function theme_ogdi_field_formatter_default($element) {
  $output = '';
  // If there is a dataset to load 
  if (!empty($element['#item']['value'])) {

    /* Add the necessary javascript and css files...*/
    /* Jquery UI */
    drupal_add_css(drupal_get_path('module', 'jquery_ui') ."/jquery.ui/themes/base/ui.all.css", 'module', 'all');
  
    /* DataTables CSS and JS */
    drupal_add_js(drupal_get_path('module', 'ogdi_field') .'/plugins/dataTables/media/js/jquery.dataTables.min.js', 'module');
    drupal_add_css(drupal_get_path('module', 'ogdi_field') .'/plugins/dataTables/media/css/demo_table.css', 'module', 'all');
    drupal_add_css(drupal_get_path('module', 'ogdi_field') .'/plugins/dataTables/media/css/demo_table_jui.css', 'module', 'all');
  
    /* Add bing maps stuff */
    drupal_set_html_head('<script type="text/javascript" src="http://ecn.dev.virtualearth.net/mapcontrol/mapcontrol.ashx?v=6.3"></script>');
  
    /* OGDI Stuff */
    drupal_add_js(drupal_get_path('module', 'ogdi_field') .'/js/ogdi_field.js', 'module');
    drupal_add_css(drupal_get_path('module', 'ogdi_field') ."/css/ogdi_field.css", 'module', 'all');
  
    /* Reset the sticky table headers - see theme_ogdi_field_container() */
    drupal_add_js('$("table.no-sticky thead").addClass("tableHeader-processed");', 'inline', 'footer');

    // Filter out any harmful stuff
    $field_url = filter_xss($element['#item']['safe']);
    
    // Get the ID of the actual Dataset, should be the last element in the array
    $url = parse_url($field_url);
    $dataset_path = explode('/', $url['path']); // explode('/', substr($field_url, 7));
    $dataset_id = $dataset_path[count($dataset_path) - 1]; 
    
    // Get the path to the actual Datasource instance
    $datasource = $url['scheme'] . '://' . $url['host'] . '/' . implode('/', array_slice($dataset_path, 1, -1));
    
    // Create a new instance of OGDI object
    $ogdi = ogdi_field_ogdi($datasource, $dataset_id);
    
    // Get the schema (column names) of the dataset
    $schema = ogdi_field_get_metadata($ogdi);
    
    // Build the table headers from that
    $headers = ogdi_field_build_table_headers($schema->d[0]);
    
    // Get the data
    $parameters = array('format' => 'json');
    $data = ogdi_field_get_data($ogdi, $parameters);
    
    // Build the table rows from that data
    $rows = ogdi_field_build_table($data->d, $headers);
        
    // Build the filter form
    $filter_form = drupal_get_form('ogdi_field_filter_form', $ogdi, $element);
    $output .= $filter_form;
    
    // Theme the container
    $output .= theme('ogdi_field_container', $ogdi, $headers, $rows, $parameters);
  } 
  return $output;
}


/**
 * Theme function for 'plain' ogdi_field field formatter.
 */
function theme_ogdi_field_formatter_plain($element) {
  return strip_tags($element['#item']['safe']);
}

/**
 * Theme function for 'link' ogdi_field field formatter.
 */
function theme_ogdi_field_formatter_link($element) {
  return l(strip_tags($element['#item']['safe']), strip_tags($element['#item']['safe']));
}


/**
 * Theme function for OGDI data grid and Map in tabs
 */
function theme_ogdi_field_container($ogdi, $headers, $rows, $parameters) {
  // Add the jQuery tabs functionality
  if (module_exists('jquery_ui')) {
    jquery_ui_add('ui.tabs');
  }

  $output .= '<div id="ogdi-wrapper">' . chr(10); 
    $output .= '<div id="ogdi-tabs">'. chr(10);
      // Build the tabs...
      $output .= '<ul>'. chr(10);
      $output .= '<li><a href="#ogdi-data">Data</a></li>' . chr(10);;
      $output .= '<li><a href="#ogdi-map">Map</a></li>' . chr(10);;
      $output .= '</ul>'. chr(10);
      // Output the data table...
      $output .= '<div id="ogdi-data">' . chr(10);
      $output .= theme('ogdi_field_datatable', $headers, $rows, $ogdi->datasource, $ogdi->dataset_id, $parameters);
      $output .= '</div>' . chr(10);
      // Output the map...
      $output .= '<div id="ogdi-map">' . chr(10);
      $output .= theme('ogdi_field_map', $ogdi->datasource, $ogdi->dataset_id, $parameters);
      $output .= '</div>' . chr(10);
    $output .= '</div>' . chr(10);
  $output .= '</div>' . chr(10);
  return $output;
}


/**
 * Theme function for the OGDI data grid
 */
function theme_ogdi_field_datatable($headers, $rows, $datasource, $dataset_id, $parameters) {
  foreach ($parameters as $key => $value) {
      if ($value == 'json') {
        continue;
      }
      $querystring[$key] = $value;
    }
 
  // Output the Path to the ATOM Feed API
  $output .= theme('ogdi_field_atom_link', $datasource, $dataset_id, $querystring);

  // Output the Download select box
  $output .= theme('ogdi_field_download_link', $datasource, $dataset_id, $querystring);
  
  // If no data is returned...
  if (count($rows) == 0) {
    $output = '<div id="ogdi-datagrid">' . t('No data could be found, please try again') . '</div>';
  } 
  else {      
    $output .= theme('table', $headers, $rows, $attributes = array('id' => 'ogdi-datagrid', 'class' => 'no-sticky'));
  }
  return $output;
}


/**
 * Theme function for the OGDI Map
 */
function theme_ogdi_field_map($datasource, $dataset_id, $parameters) {
  $parameters['format'] = 'kml';
  foreach ($parameters as $key => $value) {
      $string[] = $key .'='. urlencode($value);
    }
  $querystring = implode('&', $string);
  
  // Output the Path to the ATOM Feed API
  $output .= theme('ogdi_field_atom_link', $datasource, $dataset_id, $querystring);
  
  $output .= '<div id="bing-map">';
  $output .= $datasource . '/' . $dataset_id .'?'. $querystring .'</div>';
  return $output;
}


/**
 * Theme function for the OGDI Filter helptext link and modal window
 */
function theme_ogdi_field_helptext($field) {
  $output = '';
  // If there is a value in the field, we want to ouput it..
  if (!empty($field)) {
    if (module_exists('jquery_ui')) {  // If jquery_ui is installed(which it should be)...
      jquery_ui_add('ui.dialog'); // Load the dialog widget...
      $current_page = drupal_get_path_alias($_GET['q']);
      $output .= l('Help on how to write expressions', $current_page, $options = array('attributes' => array('class' => 'ogdi-helptip'), 'html' => TRUE));  // Output the help icon...
    }
    $output .= '<div class="ogdi-helptext" title="' . t('Expression filter hints') . '"' . chr(10);
    $output .= $field['#value'] . chr(10);
    $output .= '</div>' . chr(10) ;
  }
  return $output;
}


/**
 * Theme function to output the Path to the ATOM Feed API of the visible data
 */
function theme_ogdi_field_atom_link($datasource, $dataset_id, $querystring) {
  // Output the Path to the ATOM Feed API
  $url = url($datasource . '/' . $dataset_id, $options = array('html' => TRUE, 'query' => $querystring));
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
function ogdi_field_filter_form(&$form_state, $ogdi, $element) {
  // Get the current field so that we can set the titles and descriptions on form elements...
  $field = content_fields($element['#field_name']);

  $form['query'] = array(
    '#title' => t('Filter the dataset'),
    '#type' => 'fieldset',
    '#collapsed' => TRUE,
    '#collapsible' => TRUE
  );  
  
  $form['query']['filter'] = array(
    '#type'         =>  'textarea',
    '#title'        =>  $field['filter_label'],
    '#description'  =>  $field['filter_description'],
    '#element_validate' =>  array('ogdi_field_filter_validate')
  );
  
  $form['query']['helptext'] = array(
    '#value'        =>  $field['filter_helptext'],
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
    '#ahah' => array(
      'path' => 'ogdi_field_filter',  // See hook_menu for this...
      'wrapper' => 'ogdi-wrapper',
      'progress' => array('type' => 'throbber', 'message' => t('Please wait...')),
      'method' =>  'replace',
      'effect' =>  'fade'
     ),
  );
  
  return $form;
}