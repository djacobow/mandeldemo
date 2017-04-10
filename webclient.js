
config = {
 x_tiles: 4,
 y_tiles: 4,

};


var params = {
    max_iters: 255,
    escape_val: 2,
    x_pels: 1024,
    y_pels: 1024,
    x_min: -2,
    x_max: 2,
    y_min: -2,
    y_max: 2,
};

function handleReturnedData(res) {
    console.log(res.params);

    var canvas = document.getElementById('viewport');
    var ctx = canvas.getContext('2d');

    var x_min_pel = params.x_pels * res.params.x_tile / config.x_tiles;
    var y_min_pel = params.y_pels * res.params.y_tile / config.y_tiles;

    var iData = ctx.getImageData(x_min_pel,y_min_pel,res.params.x_pels,res.params.y_pels);
    var tile_x = 0;
    var tile_y = 0;

    for (var i=0; i< res.data.length; i++) {
        var dv = res.data[i];
        iData.data[4*i + 0] = dv;
        iData.data[4*i + 1] = dv < 127 ? dv : 255 - dv;
        iData.data[4*i + 2] = params.max_iters - dv;
        iData.data[4*i + 3] = 0xff;
    }

    ctx.putImageData( iData, x_min_pel, y_min_pel); 
}

function submitJobs() {

    /*
    var form_params = ['x_min', 'x_max', 'y_min', 'y_max'];
    for (var i=0;i<form_params.length;i++) {
        params[form_params[i]] = parseFloat(document.getElementById(form_params[i]).value);
    }
    */

    var x_width  = params.x_max - params.x_min;
    var y_height = params.y_max - params.y_min;

    var tilenum = 0;

    for (var yt=0; yt<config.y_tiles; yt++) {
        var tile_y_min = params.y_min + yt * y_height / config.y_tiles;
        var tile_y_max = tile_y_min + y_height / config.y_tiles;
        var tile_y_pels = params.y_pels / config.y_tiles;

        for (var xt=0; xt < config.x_tiles; xt++) {
            var tile_x_min = params.x_min + xt * x_width / config.x_tiles;
            var tile_x_max = tile_x_min + x_width / config.x_tiles;
            var tile_x_pels = params.x_pels / config.x_tiles;

            
            var pkeys = Object.keys(params);
            var tile_params = {};
            for (var n=0;n<pkeys.length;n++) {
                tile_params[pkeys[n]] = params[pkeys[n]];
            }

            tile_params.x_min = tile_x_min;
            tile_params.y_min = tile_y_min;
            tile_params.x_max = tile_x_max;
            tile_params.y_max = tile_y_max;
            tile_params.y_pels = tile_y_pels;
            tile_params.x_pels = tile_x_pels;
            tile_params.x_tile = xt;
            tile_params.y_tile = yt;

            var xhr = new XMLHttpRequest();
            /* jslint loopfunc:true */
            xhr.addEventListener('load', function(evt) {
                handleReturnedData(JSON.parse(evt.currentTarget.response));
            });
            var urlchunks = ['render/?'];
            pkeys = Object.keys(tile_params);
            for (var m=0;m<pkeys.length;m++) {
                urlchunks.push(pkeys[m] + '=' + tile_params[pkeys[m]]);
                if (m < pkeys.length-1) urlchunks.push('&');
            }
            var final_url = urlchunks.join('');
            console.log('FINAL_URL: ' + final_url);
            xhr.open('GET',final_url);
            xhr.send();
            tilenum += 1;
        }
    }


}

document.getElementById('resetbutton').addEventListener('click',function() {
    params.x_min = -2;
    params.x_max = 2;
    params.y_min = -2;
    params.y_max = 2;
    submitJobs();
});


var canvas = document.getElementById('viewport');

canvas.addEventListener('mousedown', function(evt) {
    storePort('down', evt);
});
canvas.addEventListener('mouseup', function(evt) {
    storePort('up', evt);
});

    
var userRange = {};

function storePort(which, evt) {    
    var rect = canvas.getBoundingClientRect();
    var x = evt.clientX - rect.left;
    var y = evt.clientY - rect.top;
    if (which == 'down') {
        userRange.down = { x:x, y:y };
    }
    if (which == 'up') {
        userRange.up = { x:x, y:y };
        for (var w in ['x','y']) {
            if (userRange.down[w] > userRange.up[w]) {
                var t = userRange.up[w];
                userRange.up[w] = userRange.down[w];
                userRange.down[w] = t;
            }
        }
        console.log(userRange);

        var new_min_x = params.x_min + 
                        (params.x_max - params.x_min) * (userRange.down.x / params.x_pels);
        var new_max_x = params.x_min +
                        (params.x_max - params.x_min) * (userRange.up.x / params.x_pels);
        var new_min_y = params.y_min + 
                        (params.y_max - params.y_min) * (userRange.down.y / params.y_pels);
        var new_max_y = params.y_min +
                        (params.y_max - params.y_min) * (userRange.up.y / params.y_pels);

        params.x_min = new_min_x;
        params.x_max = new_max_x;
        params.y_min = new_min_y;
        params.y_max = new_max_y;
        submitJobs();

    }
}



