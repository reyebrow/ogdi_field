// $Id$
(function ($) {
  //==========================================//
  // BUILDING THE TABS
  //==========================================//
  Drupal.behaviors.buildTabs = {
    attach: function (context, settings) {
      if($('#ogdi-wrapper').length > 0) {
        
        //==========================================//
        // BUILDING THE MAP
        //==========================================//
        var url = $('#bing-map').text();
        //var title = Drupal.settings.ogdi_field.title;
        //var description = Drupal.settings.ogdi_field.description;
        $('#bing-map').gmap({'credentials': 'AoMbZ1CHE4fV_eihmcvW8m19IT4Gbn2oago4voVygVsAWwM8nN0aEmOen1Tc68xa', 'enableSearchLogo': false}).bind('init', function() {
        	$.getJSON(url, function(response) {
        		$.each(response.d, function(i, obj) {
        			// Get our location from the fields we defined as lat long in the module
              var location = new Microsoft.Maps.Location(obj[Drupal.settings.ogdi_field.latitude], obj[Drupal.settings.ogdi_field.longitude]);
              // Add markers at our location
        			$('#bing-map').gmap('addMarker', { 'location': location, 'bounds': true } )
        			.click(function() {
                // Set up the marker/pin information...
        				$('#ogdi-map #bing-map').gmap('openInfoWindow', { 
        					'title': obj[Drupal.settings.ogdi_field.title],
                  'description': obj[Drupal.settings.ogdi_field.description]
        				}, this);
        			});
        		});
        	});
        });

        // Create the tabs...   
        $("#ogdi-tabs").tabs();
        
        // Build the datagrid...
        $('#ogdi-datagrid').dataTable({
          "bJQueryUI": true,  // Add support for jQueryUI...
          "bAutoWidth": true, // Allow it's width to be automatic...
          "sPaginationType": "full_numbers",  // Use full numbers for pagination...
          "sScrollX": "100%", // Provide a horizontal scrollbar...
          "bScrollCollapse": true,
          "bRetrieve" : true
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