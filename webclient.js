
config = {
 x_tiles: 10,
 y_tiles: 10,
 sat: 0.5,
 lum: 0.5,
 // starter params
 x_min: -3,
 x_max: 3,
 y_min: -3,
 y_max: 3,
};

var roundBy = function(a,r) {
    return r * Math.floor(a / r);
};

var params = {
    max_iters: 255,
    escape_val: 20,
    x_pels: roundBy(1024, 20),
    y_pels: roundBy(576, 20),
    type: 0,
    do_julia: 0,
    jx: 0.285,
    jy: 0.01,
    do_local: 0,
};

var userRange = {};


/**
 * Converts an HSL color value to RGB. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSL_color_space.
 * Assumes h, s, and l are contained in the set [0, 1] and
 * returns r, g, and b in the set [0, 255].
 *
 * @param   {number}  h       The hue
 * @param   {number}  s       The saturation
 * @param   {number}  l       The lightness
 * @return  {Array}           The RGB representation
 */
/* cribbed from http://stackoverflow.com/questions/2353211/hsl-to-rgb-color-conversion */
function hslToRgb(h, s, l){
    var r, g, b;
    if(s === 0){
        r = g = b = l; // achromatic
    }else{
        var hue2rgb = function hue2rgb(p, q, t){
            if(t < 0) t += 1;
            if(t > 1) t -= 1;
            if(t < 1/6) return p + (q - p) * 6 * t;
            if(t < 1/2) return q;
            if(t < 2/3) return p + (q - p) * (2/3 - t) * 6;
            return p;
        };

        var q = l < 0.5 ? l * (1 + s) : l + s - l * s;
        var p = 2 * l - q;
        r = hue2rgb(p, q, h + 1/3);
        g = hue2rgb(p, q, h);
        b = hue2rgb(p, q, h - 1/3);
    }

    return [Math.round(r * 255), Math.round(g * 255), Math.round(b * 255)];
}

function handleReturnedData(res) {
    // console.log(res.params);

    var canvas = document.getElementById('viewport');
    var ctx = canvas.getContext('2d');

    var x_min_pel = params.x_pels * res.params.x_tile / config.x_tiles;
    var y_min_pel = params.y_pels * res.params.y_tile / config.y_tiles;

    var iData = ctx.getImageData(x_min_pel,y_min_pel,res.params.x_pels,res.params.y_pels);
    var tile_x = 0;
    var tile_y = 0;

    for (var i=0; i< res.data.length; i++) {
        var dv = res.data[i];
        if (false) {
            if (params.max_iters != 255) dv *= (255.0 / params.max_iters);
            iData.data[4*i + 0] = dv < 128 ? 2 * dv : 255;
            iData.data[4*i + 1] = dv < 64  ? 4 * dv : 255;
            iData.data[4*i + 2] = dv < 84  ? 3 * dv : 255;
        } else if (false){
            if (params.max_iters != 255) dv *= (255.0 / params.max_iters);
            iData.data[4*i + 0] = Math.sqrt(dv) * 16.0;
            iData.data[4*i + 1] = 0;
            iData.data[4*i + 2] = dv * dv / 256;
        } else {
            var color = [];
            if (dv == params.max_iters) {
                color = [0,0,0];
            } else {
                color = hslToRgb(dv/params.max_iters, config.sat, config.lum);
            }
            iData.data[4*i + 0] = color[0];
            iData.data[4*i + 1] = color[1];
            iData.data[4*i + 2] = color[2];
        }
        iData.data[4*i + 3] = 0xff;
    }

    ctx.putImageData( iData, x_min_pel, y_min_pel); 
}

function shuffle(a) {
    var j, x, i;
    for (i = a.length; i; i--) {
        j = Math.floor(Math.random() * i);
        x = a[i - 1];
        a[i - 1] = a[j];
        a[j] = x;
    }
}

function calc_fractal(inurl,cb) {
    inurl = inurl.replace(/^.*\?/,'');
    inargs = inurl.split(/&/);
    var args = {};
    for (var i=0; i<inargs.length;i++) {
        var inarg = inargs[i];
        var aps = inarg.split(/=/);
        args[aps[0]] = parseFloat(aps[1]);
    }

    var data = generate_fractal(args);

    var rv = {
        params: args,
        data: data,
    };
    return cb(rv);
}

function submitJobs() {

    setDisplayWindow(params.x_min,params.y_min,params.x_max,params.y_max);

    /*
    var form_params = ['x_min', 'x_max', 'y_min', 'y_max'];
    for (var i=0;i<form_params.length;i++) {
        params[form_params[i]] = parseFloat(document.getElementById(form_params[i]).value);
    }
    */

    var x_width  = params.x_max - params.x_min;
    var y_height = params.y_max - params.y_min;

    var tilenum = 0;

    var render_urls = [];
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
            tile_params.x_pels = tile_x_pels;
            tile_params.y_pels = tile_y_pels;
            tile_params.x_tile = xt;
            tile_params.y_tile = yt;

            var urlchunks = ['render/?'];
            pkeys = Object.keys(tile_params);
            for (var m=0;m<pkeys.length;m++) {
                urlchunks.push(pkeys[m] + '=' + tile_params[pkeys[m]]);
                if (m < pkeys.length-1) urlchunks.push('&');
            }
            var final_url = urlchunks.join('');
            console.log('FINAL_URL: ' + final_url);
            render_urls.push(final_url);
            tilenum += 1;
        }
    }

    // make it seem more like stuff is happening randomly
    // Browsers limit connections to just a few, so the jobs 
    // are still sent out more or less sequentially and will
    // finish more or less sequentially
    shuffle(render_urls);

    for (var i=0; i<render_urls.length; i++) {
        var render_url = render_urls[i];
        if (params.do_local) {
            calc_fractal(render_url,handleReturnedData);
        } else {
            var xhr = new XMLHttpRequest();
            /* jslint loopfunc:true */
            xhr.addEventListener('load', function(evt) {
                handleReturnedData(JSON.parse(evt.currentTarget.response));
            });
            xhr.open('GET',render_url);
            xhr.send();
        }
    }

}


function makeCalcWindowSquareWithViewport() {
    // this sets the calculation window to match the aspect ratio of the
    // real window. This keeps the resulting image 'square'.
    var x_center = (params.x_min + params.x_max) / 2;
    var y_center = (params.y_min + params.y_max) / 2;
    // Keep x zoom where it is, but make y zoom adjust to new
    // viewport
    var yx_ratio = params.y_pels / params.x_pels;
    var x_width  = params.x_max - params.x_min;
    var y_width  = x_width * yx_ratio;
    var new_y_min = y_center - 0.5 * y_width;
    var new_y_max = y_center + 0.5 * y_width;
    params.y_min = new_y_min;
    params.y_max = new_y_max;
}

function resetWindowWithViewport() {
    console.log('resetWindowWithViewPort();');
    params.x_min = config.x_min;
    params.x_max = config.x_max;
    params.y_min = config.y_min;
    params.y_max = config.y_max;
    makeCalcWindowSquareWithViewport();
}

function do_reset() {
    resetWindowWithViewport();
    submitJobs();
}


    

function storePort(which, evt) {    
    var canvas = document.getElementById('viewport');
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
        // console.log(userRange);


        var new_min_x = params.x_min + 
                        (params.x_max - params.x_min) * (userRange.down.x / params.x_pels);
        var new_max_x = params.x_min +
                        (params.x_max - params.x_min) * (userRange.up.x / params.x_pels);
        var new_min_y = params.y_min + 
                        (params.y_max - params.y_min) * (userRange.down.y / params.y_pels);
            new_max_y = params.y_min +
                        (params.y_max - params.y_min) * (userRange.up.y / params.y_pels);

        params.x_min = new_min_x;
        params.x_max = new_max_x;
        params.y_min = new_min_y;
        params.y_max = new_max_y;

        var force_aspect_ratio_fixed = true;
        if (force_aspect_ratio_fixed) {
            makeCalcWindowSquareWithViewport();
        }

        submitJobs();

    }
}

function setDisplayWindow(x1,y1,x2,y2) {
    console.log('setDisplayWindow();');
    document.getElementById('xmin').value = x1.toString();
    document.getElementById('ymin').value = y1.toString();
    document.getElementById('xmax').value = x2.toString();
    document.getElementById('ymax').value = y2.toString();
}

function getParamsFromPage() {
    var type = parseInt(document.getElementById('typesel').value);
    params.type = type;
    var tilex = parseInt(document.getElementById('tilex').value);
    config.x_tiles = tilex;
    var tiley = parseInt(document.getElementById('tiley').value);
    config.y_tiles = tiley;
    var ev = parseInt(document.getElementById('escape').value);
    params.escape_val = ev;
    var mi = parseInt(document.getElementById('maxiters').value);
    params.max_iters = mi;
    var sat = parseFloat(document.getElementById('color_sat').value);
    config.sat = sat;
    var lum = parseFloat(document.getElementById('color_lum').value);
    config.lum= lum;

    var xmin = parseFloat(document.getElementById('xmin').value);
    var xmax = parseFloat(document.getElementById('xmax').value);
    var ymin = parseFloat(document.getElementById('ymin').value);
    var ymax = parseFloat(document.getElementById('ymax').value);
    params.x_min = xmin;
    params.x_max = xmax;
    params.y_min = ymin;
    params.y_max = ymax;

    var julia = document.getElementById('julia').checked ? 1 : 0;
    params.do_julia = julia;
    params.jx = parseFloat(document.getElementById('jx').value);
    params.jy = parseFloat(document.getElementById('jy').value);
    var local = document.getElementById('local').checked ? 1 : 0;
    params.do_local = local;
    makeCalcWindowSquareWithViewport();
    submitJobs();
}


function adjustViewport() {
  console.log('resize');
  var canvas = document.getElementById('viewport');
  var ctx = canvas.getContext('2d');
  var round = 20;
  x = roundBy(0.9 * window.innerWidth, round);
  y = roundBy(0.9 * window.innerHeight, round);
  params.x_pels = x;
  params.y_pels = y;
  ctx.canvas.width  = x;
  ctx.canvas.height = y;
  makeCalcWindowSquareWithViewport();
  console.log(params);
}

function doOnceAtStart() {
    document.getElementById('resetbutton').addEventListener('click',do_reset);

    var resize_interval_id;
    window.addEventListener('resize', function() {
        clearTimeout(resize_interval_id);
        resize_interval_id = setTimeout(function() {
            adjustViewport();
            submitJobs();
        },500);
    });

    var ids = ['typesel','tilex','tiley','escape','maxiters','color_sat',
               'color_lum','xmin','xmax','ymin','ymax','julia','jx','jy',
               'local'];
    for (var i=0; i<ids.length; i++) {
        document.getElementById(ids[i]).addEventListener('change',getParamsFromPage);
    }

    // getParamsFromPage();
    console.log(params);
    adjustViewport();
    do_reset();

    var canvas = document.getElementById('viewport');
    canvas.addEventListener('mousedown', function(evt) {
        evt.preventDefault();
        storePort('down', evt);
    });
    canvas.addEventListener('mouseup', function(evt) {
        storePort('up', evt);
    });

    canvas.addEventListener('touchstart', function(evt) {
        evt.preventDefault();
        console.log('touchStart()');
        storePort('down',evt.changedTouches[0]);
    });

    canvas.addEventListener('touchend', function(evt) {
        evt.preventDefault();
        console.log('touchEnd()');
        storePort('up',evt.changedTouches[0]);
    });
}

doOnceAtStart();

