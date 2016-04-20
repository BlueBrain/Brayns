/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Nicolas Antille <nicolas.antille@epfl.ch>
 *
 * This file is part of BRayns
 */

var redirect = function(tgt)
{
    var msg = 
    {
        eventName: 'location',
        data:
        {
            url: tgt
        }
    };
    window.parent.postMessage(msg, '*');
}

// With JQuery
$('#timer').slider({
	formatter: function(value) {
		return 'Time: ' + value;
	}
});

$.fn.bootstrapSwitch.defaults.size = 'mini';
$("[id='shading']").bootstrapSwitch();
$("[id='shadows']").bootstrapSwitch();
$("[id='electronshading']").bootstrapSwitch();
$("[id='ambientocclusion']").bootstrapSwitch();
$("[id='detectiononmaterials']").bootstrapSwitch();

$('.dropdown-menu a').on('click', function(){    
    $(this).parent().parent().prev().html($(this).html() + ' <span class="caret"></span>');    
})

$("#timer").on("slide", function(slideEvt) {
	$("#timerSliderVal").text(slideEvt.value);
});

$("document").ready(function() 
{
    $('.start').click(function()
    {
        $('html, body').animate({scrollTop: $(".platform").offset().top}, 800);
    });
});