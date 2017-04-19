
function cmag(z)    { return Math.sqrt(z.r*z.r + z.i*z.i); }
function csmul(z,s) { return { r: z.r * s, i: z.i * s };   }
function ccube(z)   {
    return {
        r: z.r*z.r*z.r - 3*z.r*z.i*z.i,
        i: 3*z.r*z.r*z.i - z.i*z.i*z.i,
    };
}
function cadd(z,y)  {
    return {
        r: z.r + y.r,
        i: z.i + y.i,
    };
}
function csquare(z) {
    return {
        r: z.r*z.r - z.i*z.i,
        i: 2*z.r*z.i,
    };
}

function znp1_mb(z,c) {
    var nzr = csquare(z);
    return cadd(nzr,c);
}
function znp1_cos(z,c) {
    var nz = { r:0, i:0 };
    nz.r = Math.cos(z.r) * Math.cosh(z.i) + c.r;
    nz.i = Math.sin(z.r) * Math.sinh(z.i) + c.i;
    return nz;
}
function znp1_mbtc(z,c) {
    z.i = -z.i;
    return cadd(csquare(z),c);
}
function znp1_mbbs(z,c) {
    z.r = Math.abs(z.r);
    z.i = Math.abs(z.i);
    return cadd(csquare(z),c);
}
function znp1_mb3(z,c) {
    return cadd(ccube(z),c);
}
function znp1_nf3(z,c) {
    var nz = ccube(z);
    nz.r += c.r - 1.0;
    nz.i += c.i;
    return nz;
}
function znp1_nf3m2z(z,c) {
    return cadd(cadd(ccube(z),csmul(z,-2)),c);
}
function znp1_nf8p15z4(z,c) {
    var z2 = csquare(z);
    var z4 = csquare(z2);
    var z8 = csquare(z4);
    var z14_15 = csmul(z4,15);
    var nz = cadd(z8,z14_15);
    return cadd(nz,c);
}

function calc_pixel_generic(args,x,y) {
    c = { r:0, i: 0};
    z = { r:0, i: 0};
    if (args.do_julia) {
        z.r = x; z.i = y; 
        c.r = args.jx; c.i = params.jy;
    } else {
        z.r = 0; z.i = 0;
        c.r = x; c.i = y;
    }
    var iter = 0;
    var val = 0;
    while ((iter < args.max_iters) && (val < params.escape_val)) {
        var nz = args.algo(z,c);
        val = cmag(nz);
        z = nz;
        iter++;
    }
    return iter;
}

function generate_fractal(args) {
    console.log('generate_fractal()');
    console.log(args);
    var x_step = (args.x_max - args.x_min) / args.x_pels;
    var y_step = (args.y_max - args.y_min) / args.y_pels;

    var data = new Array(args.x_pels * args.y_pels);
    var algo = znp1_mb;

    switch (args.type) {
        case 1 : algo = znp1_mb3; break;
        case 2 : algo = znp1_mbbs; break;
        case 3 : algo = znp1_mbtc; break;
        case 4 : algo = znp1_cos; break;
        case 5 : algo = znp1_nf3; break;
        case 6 : algo = znp1_nf3m2z; break;
        case 7 : algo = znp1_nf8p15z4; break;
        default: algo = znp1_mb; break;
    }

    args.algo = algo;

    for (var j=0; j<args.y_pels; j++) {
        var y = args.y_min + j * y_step;
        for (var i=0; i<args.x_pels; i++) {
            var x = args.x_min + i * x_step;
            var iters = 0;
            var val = 0;
            iters = calc_pixel_generic(args, x, y);
            data[i + j * args.x_pels] = iters;
        }
    }
    console.log('generate_done');
    return data;
}

