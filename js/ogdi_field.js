// $Id$
(function ($) {
  //==========================================//
  // BUILDING THE TABS
  //==========================================//
  Drupal.behaviors.buildTabs = {
    attach: function (context, settings) {
      if($('#ogdi-wrapper').length > 0) {
        // Build the Map - this stuff must come first..
        var container = $('#ogdi-map #bing-map'); // Get the map container...
        var url = container.text(); // Get the URL of the kml document...
        var map;  // Set a map variable...
        map = new VEMap('bing-map');  // Create a new map in the container <div id="bing-map">
        map.LoadMap();  // Load the map...
        var shapeLayer = new VEShapeLayer();  // Create a shape layer to import the KML data into...
        var shapeSpec = new VEShapeSourceSpecification(VEDataType.ImportXML, url, shapeLayer);
        map.ImportShapeLayerData(shapeSpec);
      
        // Create the tabs...   
        $("#ogdi-tabs").tabs();
        
        // Build the datagrid...
        $('#ogdi-datagrid').dataTable({
          "bJQueryUI": true,  // Add support for jQueryUI...
          "bAutoWidth": true, // Allow it's width to be automatic...
          "sPaginationType": "full_numbers",  // Use full numbers for pagination...
          "sScrollX": "100%", // Provide a horizontal scrollbar...
          "bScrollCollapse": true
        });
      }
    }
  }
  
  //==========================================//
  // CREATING THE DIALOG BOX
  //==========================================//
  Drupal.behaviors.dialog = {
    attach: function (context, settings) {
      // If the dialog text exists...
      if($('div.ogdi-helptext').length > 0) {
        // Set a variable to use later...
        var $dialog = $('div.ogdi-helptext');
        // Hide the container...
        $dialog.hide();
        // Get the tooltip link...
        var $tooltip = $('a.ogdi-helptip');
        // On click
        $tooltip.click(function() {
          $dialog.clone().dialog({  // Clone the dialog container to prevent the dialog() function from eating it...
            width: 680,
            modal: true
          });
          return false;
        });
      }
    }
  }
})(jQuery);