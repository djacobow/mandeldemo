#!/home/pi/.nvm/versions/node/v10.0.0/bin/node

var connect = require('connect');
var compression = require('compression');
var url = require('url');
var fr = require('./build/Debug/fractizer');


function setParamTypes(inparms) {
    cmd = "./fractizer";
    argnames = [
        [ 'max_iters', 'i', ],
        [ 'escape_val', 'f', ],
        [ 'x_min', 'f', ],
        [ 'x_max', 'f', ],
        [ 'y_min', 'f', ],
        [ 'y_max', 'f', ], 
        [ 'x_pels', 'i' ],
        [ 'y_pels', 'i' ],
        [ 'x_tile', 'i' ],
        [ 'y_tile', 'i' ],
        [ 'type', 'i' ],
        [ 'do_julia', 'i' ],
        [ 'jx', 'f' ],
        [ 'jy', 'f' ],
    ];

    outparms = {};
    argnames.forEach((a) => {
        var aname = a[0];
        var atype = a[1];
        if (inparms.hasOwnProperty(aname)) {
            outparms[aname] = atype == 'i' ? parseInt(inparms[aname]) : parseFloat(inparms[aname]);
        };
    });

    console.log(outparms);
    return outparms;
}

function getParamsFromUrl(inurl) {
    console.log(inurl);
    var sp = url.parse(inurl).query;
    console.log(sp);
    var res = {};
    try {
        var chunks = sp.split(/&/);
        for (var i=0; i< chunks.length; i++) {
            var nv = chunks[i].split(/=/);
            res[nv[0]] = nv[1];
        }
    } catch(e) {
        console.error(e);
    }
    return res;
}

var app = connect();
app.use(compression({ filter: function(req,res) { return true; } }));

app.use(function(req, res) {
    console.log('got request');
    var parms0 = getParamsFromUrl(req.url);
    var parms1 = setParamTypes(parms0);

    var headers = {
        'Content-Type': 'application/json',
        'Access-Control-Allow-Origin': '*',
    };

    
    fr.aRun(parms1,(err,data) => {
        var out = { params: parms1, };

        if ((data === null) || (typeof data === 'undefined')) {
            out.data = [];
        } else {
            out.data = Array.from(data);
        }
        var rstr  = JSON.stringify(out);
        res.writeHead(200,headers);
        res.end(rstr);
    });
});

app.listen(5001);
