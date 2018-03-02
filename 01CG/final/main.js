<script>
	function Color(r, g, b) {
	    this.r = r;
	    this.g = g;
	    this.b = b;
	}
</script>
<script src="js/color_picker.js" type="text/javascript"></script>
<script src="js/jquery-1.6.1.min.js"></script>
<script src="js/ace/ace.js" type="text/javascript" charset="utf-8"></script>
<script src="js/ace/theme-clouds_midnight.js" type="text/javascript" charset="utf-8"></script>
<script src="js/ace/mode-glsl.js" type="text/javascript" charset="utf-8"></script>
<script src="js/super_picker.js" type="text/javascript"></script>
<script src="js/super_slider.js" type="text/javascript"></script>
<script src="js/jquery.mousewheel.min.js" type="text/javascript"></script>
<script src="js/macton/macton-utils.js" type="text/javascript"></script>
<script src="js/macton/macton-gl-utils.js" type="text/javascript"></script>
<script src="js/macton/webgl-utils.js" type="text/javascript"></script>
<script src="js/macton/matrix4x4.js" type="text/javascript"></script>
<script src="js/macton/cameracontroller.js" type="text/javascript"></script>
<script src="js/jquery-ui-1.8.13.custom.min.js" type="text/javascript"></script>
<script src="js/ltc_tables.js" type="text/javascript"></script>
<script>
var g_vshader = null;

var g_param_types = {};
var g_params = {};
var g_param_edited = {};
var g_param_default = {};

var g_sample_count = 0;

var bindSlider = function(name, label, default_value, value, state) {
    var slider = new SuperSlider(name, {
        label: label,
        default_value: default_value,
        value: value,
        min:  state.min,
        max:  state.max,
        step: state.step });

    slider.bind("change", function(event) {
        g_params[name] = event.target.val;
        g_param_edited[name] = true;
        g_sample_count = 0;
    });

    slider.bind("reset", function(event) {
        g_param_edited[name] = undefined;
        g_sample_count = 0;
    });

    return slider;
}

var bindPicker = function(name, label, default_value, value) {
    var picker = new SuperPicker(name, {
        label: label,
        default_value: default_value,
        value: value,
        callback: function(col) {
            g_params[name] = col;
            g_param_edited[name] = true;
            g_sample_count = 0;
        }
    });

    picker.bind("reset", function (event) {
        g_param_edited[name] = undefined;
        g_sample_count = 0;
    });

    return picker;
}

var bindCheckbox = function(name, label, value) {
    var control = $("<input>")
		.attr({ type: "checkbox", /*id: name,*/ checked: value })
		.css("margin", "0 0 0 4px");

    var cb_label = $("<label>")
		.attr("for", name)
		.text(label)
		.addClass("checkbox_label");

    cb_label.append(control);

    control.bind("change", function (event) {
        g_params[name] = event.target.checked;
        g_param_edited[name] = true;
        g_sample_count = 0;
    });

    cb_label.bind("click", function (event) {
        control.prop("checked", g_param_default[name]);
        control.trigger("change");
        g_param_edited[name] = undefined;
        g_sample_count = 0;
    });

    control.bind("click", function (event) {
        event.stopPropagation();
    });

    return cb_label;
}
</script>
<script type="text/javascript">
/**
 * Provides requestAnimationFrame in a cross browser way.
 * http://paulirish.com/2011/requestanimationframe-for-smart-animating/
 */
if (!window.requestAnimationFrame) {
    window.requestAnimationFrame = (function() {
        return window.webkitRequestAnimationFrame ||
		window.mozRequestAnimationFrame ||
		window.oRequestAnimationFrame   ||
		window.msRequestAnimationFrame  ||
		function(/* function FrameRequestCallback */ callback, /* DOMElement Element */ element) {
		    window.setTimeout(callback, 1000/60);
		};
    })();
}
</script>
<script type="text/javascript">
var effectDiv, sourceDiv, canvas, gl, buffer,
vertex_shader, fragment_shader, currentProgram,
vertexPositionLocation, textureLocation,
parameters = { start_time: new Date().getTime(), time: 0, screenWidth: 0, screenHeight: 0 };

var g_zoom = 0;

var model      = new Matrix4x4();
var view       = new Matrix4x4();
var projection = new Matrix4x4();
var controller = null;

var ltc_mat_texture = null;
var ltc_mag_texture = null;

var rttFramebuffer = null;
var rttTexture = null;

var blit_vs = null;
var blit_fs = null;
var blitProgram = null;


function FetchFile(url, cache)
{
    var text = $.ajax({
        url:   url,
        async: false,
        dataType: "text",
        mimeType: "text/plain",
        cache: cache,
    }).responseText;

    return text;
}

function SetClampedTextureState()
{
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
}

function init() {
    vertex_shader   = FetchFile("shaders/ltc/ltc.vs", false);
    fragment_shader = FetchFile("shaders/ltc/ltc.fs", false);

    g_vshader = vertex_shader;
    $("#editor").text(fragment_shader);

    canvas = document.createElement("canvas");
    canvas.style.cssText = "border: 2px solid #404040; border-radius: 6px";

    effectDiv = document.getElementById("effect");
    effectDiv.appendChild(canvas);

    // Initialise WebGL
    try {
        gl = canvas.getContext("experimental-webgl");
    } catch(error) { }

    if (!gl) {
        alert("WebGL not supported");
        throw "cannot create webgl context";
    }

    // Check for float-RT support
    if (!gl.getExtension("OES_texture_float")) {
        alert("OES_texture_float not supported");
        throw "missing webgl extension";
    }

    if (!gl.getExtension("OES_texture_float_linear")) {
        alert("OES_texture_float_linear not supported");
        throw "missing webgl extension";
    }


    // Add enum strings to context
    if (gl.enum_strings === undefined) {
        gl.enum_strings = { };
        for (var propertyName in gl) {
            if (typeof gl[propertyName] == "number") {
                gl.enum_strings[gl[propertyName]] = propertyName;
            }
        }
    }

    // Create Vertex buffer (2 triangles)
    buffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([-1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0]), gl.STATIC_DRAW);

    // Create Program
    currentProgram = createProgram(vertex_shader, fragment_shader);

    ////
    rttFramebuffer = gl.createFramebuffer();
    gl.bindFramebuffer(gl.FRAMEBUFFER, rttFramebuffer);
    rttFramebuffer.width  = 512; // FIXME
    rttFramebuffer.height = 512;

    rttTexture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, rttTexture);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, rttFramebuffer.width, rttFramebuffer.height, 0, gl.RGBA, gl.FLOAT, null);

    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, rttTexture, 0);
    gl.bindFramebuffer(gl.FRAMEBUFFER, null);

    gl.bindTexture(gl.TEXTURE_2D, null);


    blit_vs = FetchFile("shaders/ltc/ltc_blit.vs", false);
    blit_fs = FetchFile("shaders/ltc/ltc_blit.fs", false);

    var header = "#ifdef GL_ES\nprecision highp float;\n#endif\n#line 0\n";
    blit_fs = header + blit_fs;

    blitProgram = gl.createProgram();

    var vs = createShader(blit_vs, gl.VERTEX_SHADER);
    var fs = createShader(blit_fs, gl.FRAGMENT_SHADER);

    gl.attachShader(blitProgram, vs);
    gl.attachShader(blitProgram, fs);

    gl.deleteShader(vs);
    gl.deleteShader(fs);

    gl.linkProgram(blitProgram);
    ////

    onWindowResize();
    window.addEventListener("resize", onWindowResize, false);

    $("canvas").mousewheel(function(event, delta) {
        g_zoom += delta*10.0;
        g_sample_count = 0;
        //		console.log("pageX: " + event.pageX + " pageY: " + event.pageY);
        return false;
    });

    $("canvas").attr("tabindex", "0").keydown(function(event) {
        //console.log(event);
        return false;
    });
	 

    ltc_mat_texture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, ltc_mat_texture);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 64, 64, 0, gl.RGBA, gl.FLOAT, new Float32Array(g_ltc_mat));
    SetClampedTextureState();

    ltc_mag_texture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, ltc_mag_texture);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.ALPHA, 64, 64, 0, gl.ALPHA, gl.FLOAT, new Float32Array(g_ltc_mag));
    SetClampedTextureState();

    // Fetch media and kick off the main program once everything has loaded
    $(function() {
        $(document).ajaxStop(function() {
            $(this).unbind("ajaxStop");
            main_prog();
        });

        // Load data here

        main_prog();
    });
}


function onWindowResize(event) {
    $("#peekaboo").css("height", window.innerHeight);
    $("#stuff").css("height", window.innerHeight);

    canvas.width  = 512; // window.innerWidth;
    canvas.height = 512; // window.innerHeight;

    parameters.screenWidth = canvas.width;
    parameters.screenHeight = canvas.height;

    gl.viewport(0, 0, canvas.width, canvas.height);
}


function glUniformTypeToString(type) {
    switch (type) {
        case gl.FLOAT:        return "FLOAT";
        case gl.FLOAT_VEC2:   return "FLOAT_VEC2";
        case gl.FLOAT_VEC3:   return "FLOAT_VEC3";
        case gl.FLOAT_VEC4:   return "FLOAT_VEC4";
        case gl.INT:          return "INT";
        case gl.INT_VEC2:     return "INT_VEC2";
        case gl.INT_VEC3:     return "INT_VEC3";
        case gl.INT_VEC4:     return "INT_VEC4";
        case gl.BOOL:         return "BOOL";
        case gl.BOOL_VEC2:    return "BOOL_VEC2";
        case gl.BOOL_VEC3:    return "BOOL_VEC3";
        case gl.BOOL_VEC4:    return "BOOL_VEC4";
        case gl.FLOAT_MAT2:   return "FLOAT_MAT2";
        case gl.FLOAT_MAT3:   return "FLOAT_MAT3";
        case gl.FLOAT_MAT4:   return "FLOAT_MAT4";
        case gl.SAMPLER_2D:   return "SAMPLER_2D";
        case gl.SAMPLER_CUBE: return "SAMPLER_CUBE";
    }

    return "unknown";
}


function createProgram(vertex, fragment) {
    var header = "#ifdef GL_ES\nprecision highp float;\n#endif\n#line 0\n";
    fragment = header + fragment;

    var program = gl.createProgram();

    var vs = createShader(vertex, gl.VERTEX_SHADER);
    var fs = createShader(fragment, gl.FRAGMENT_SHADER);

    if (vs == null || fs == null)
        return null;

    gl.attachShader(program, vs);
    gl.attachShader(program, fs);

    gl.deleteShader(vs);
    gl.deleteShader(fs);

    gl.linkProgram(program);


    if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
        /*
                alert( "ERROR:\n" +
                "VALIDATE_STATUS: " + gl.getProgramParameter( program, gl.VALIDATE_STATUS ) + "\n" +
                "ERROR: " + gl.getError() + "\n\n" +
                "- Vertex Shader -\n" + vertex + "\n\n" +
                "- Fragment Shader -\n" + fragment );
        */
        return null;
    }


    var control_names = new Array();
    var controls = {};

    var lines = fragment.split("\n");

    var bindings = { }

    // Adds double quotes around labels
    function preprocessJSON(str) {
        return str.replace(/("(\\.|[^"])*"|'(\\.|[^'])*')|(\w+)\s*:/g,
		function(all, string, strDouble, strSingle, jsonLabel) {
		    if (jsonLabel) {
		        return '"' + jsonLabel + '": ';
		    }
		    return all;
		});
    }

    for (var x in lines) {
        var line = lines[x];
        var re = /^\/\/ bind\s+(.*?)\s(.*)/;
        var m = re.exec(line);
        if (m) {
            try {
                var obj = $.parseJSON(preprocessJSON(m[2]));
                bindings[m[1]] = obj;
                control_names.push(m[1]);
            }
            catch (e) {
                console.log("Failed to parse shader binding: " + m[1]);
            }
        }
    }

    function CopyProperties(src, dest) {
        for (x in src) {
            if (dest.hasOwnProperty(x) && typeof(dest[x]) === typeof(src[x]))
                dest[x] = src[x];
        }
    }

    // Destroy existing children
    var container = $("#params");

    var children = container.children();
    children.each(function() {
        var v = $(this);
        v.detach();
    })
    container.empty();

    g_param_types = {};

    var nb_uniforms = gl.getProgramParameter(program, gl.ACTIVE_UNIFORMS);
    for (var i = 0; i < nb_uniforms; i++) {
        var uni = gl.getActiveUniform(program, i);

        var label = uni.name;
        var bind = bindings[uni.name];
        if (bind && typeof(bind.label) === "string")
            label = bind.label;

        var old_value  = g_params[uni.name];
        var is_default = !g_param_edited[uni.name];
        var new_value  = undefined;

        var control = undefined;

        if (uni.type === gl.FLOAT) {
            var state = {
                default : 1.0,
                min  : 0.0,
                max  : 1.0,
                step : 0.01,
            };

            CopyProperties(bind, state);

            var is_default = !g_param_edited[uni.name];

            new_value = is_default ? state.default : old_value;
            control = bindSlider(uni.name, label, state.default, new_value, state);
        }
        else if (uni.type === gl.FLOAT_VEC3) {
            var def_col = new Color(1, 1, 1)

            CopyProperties(bind, def_col);

            new_value = is_default ? def_col : old_value;
            control = bindPicker(uni.name, label, def_col, new_value);
        }
        else if (uni.type === gl.BOOL) {
            var def_val = true;

            if (bind && typeof(bind.default) === "boolean")
                def_val = bind.default;

            g_param_default[uni.name] = def_val;

            new_value = is_default ? def_val : old_value;
            control = bindCheckbox(uni.name, label, new_value);
        }
        else {
            continue;
        }

        g_param_types[uni.name] = uni.type;
        g_params[uni.name] = new_value;

        if (bind === undefined)
            control_names.push(uni.name);
        controls[uni.name] = control;
    }

    // Finally add the controls to the page
    for (x in control_names) {
        var control = controls[control_names[x]];

        // Skip any binds that don't reference active uniforms
        if (control === undefined)
            continue;

        // Pad for next element
        control.css("margin", "0 0 6px 0");

        container.append(control);
    }

    return program;
}


var gl_shader_error = null;

function createShader(src, type) {
    var shader = gl.createShader(type);

    gl.shaderSource(shader, src);
    gl.compileShader(shader);

    gl_shader_error = null;

    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        gl_shader_error = gl.getShaderInfoLog(shader);
        //		console.log((type == gl.VERTEX_SHADER ? "VERTEX" : "FRAGMENT") + " SHADER:\n" + gl.getShaderInfoLog(shader));
        return null;
    }

    return shader;
}


function animate() {
    requestAnimationFrame(animate);
    draw();
}

function main_prog() {
    controller = new CameraController(canvas);
    // Try the following (and uncomment the "pointer-events: none;" in
    // the index.html) to try the more precise hit detection
    //  controller = new CameraController(document.getElementById("body"), c, gl);
    controller.onchange = function(xRot, yRot) {
        draw();
        g_sample_count = 0;
    };

    requestAnimationFrame(animate);

    draw();
}


function checkGLError() {
    var error = gl.getError();
    if (error != gl.NO_ERROR) {
        var str = "GL Error: " + error + " " + gl.enum_strings[error];
        console.log(str);
        throw str;
    }
}


function draw() {
    parameters.time = new Date().getTime() - parameters.start_time;

    gl.enable(gl.DEPTH_TEST);
    gl.clearColor(0.0, 0.0, 0.0, 0.0);

    // Note: the viewport is automatically set up to cover the entire Canvas.
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    checkGLError();

    // Load program into GPU
    gl.useProgram(currentProgram);

    checkGLError();

    // Add in camera controller's rotation
    view.loadIdentity();
    view.translate(0, 6, 0.1*g_zoom - 0.5);
    view.rotate(controller.xRot - 10.0, 1, 0, 0);
    view.rotate(controller.yRot, 0, 1, 0);;

    // Get var locations
    vertexPositionLocation = gl.getAttribLocation(currentProgram, "position");

    function location(u) {
        return gl.getUniformLocation(currentProgram, u);
    }

    // Set values to program variables
    for (var x in g_params) {
        var type  = g_param_types[x];
        var value = g_params[x];
        var loc   = location(x);

        if (type === gl.FLOAT)      gl.uniform1f(loc, value);
        if (type === gl.FLOAT_VEC3) gl.uniform3f(loc, value.r, value.g, value.b);
        if (type === gl.BOOL)       gl.uniform1i(loc, value);
    }

    gl.uniformMatrix4fv(location("view"), gl.FALSE, new Float32Array(view.elements));
    gl.uniform1f(location("time"), parameters.time/1000);
    gl.uniform2f(location("resolution"), parameters.screenWidth, parameters.screenHeight);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, ltc_mat_texture);
    gl.uniform1i(gl.getUniformLocation(currentProgram, "ltc_mat"), 0);

    gl.activeTexture(gl.TEXTURE1);
    gl.bindTexture(gl.TEXTURE_2D, ltc_mag_texture);
    gl.uniform1i(gl.getUniformLocation(currentProgram, "ltc_mag"), 1);

    checkGLError();

    gl.bindFramebuffer(gl.FRAMEBUFFER, rttFramebuffer);

    if (g_sample_count === 0) {
        gl.clearColor(0, 0, 0, 0);
        gl.clear(gl.COLOR_BUFFER_BIT);
    }

    gl.uniform1i(location("sampleCount"), g_sample_count);
    g_sample_count += 8;

    gl.enable(gl.BLEND);
    gl.blendFunc(gl.ONE, gl.ONE);

    // Render geometry
    gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
    gl.vertexAttribPointer(vertexPositionLocation, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(vertexPositionLocation);
    gl.drawArrays(gl.TRIANGLES, 0, 6);
    gl.disableVertexAttribArray(vertexPositionLocation);

    gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    gl.useProgram(blitProgram);

    gl.disable(gl.BLEND);

    // Set textures
    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, rttTexture);
    gl.uniform1i(gl.getUniformLocation(blitProgram, "tex"), 0);

    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S,     gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T,     gl.CLAMP_TO_EDGE);

    gl.uniform2f(gl.getUniformLocation(blitProgram, "resolution"), parameters.screenWidth, parameters.screenHeight);

    // Blit pass
    gl.enableVertexAttribArray(vertexPositionLocation);
    gl.drawArrays(gl.TRIANGLES, 0, 6);
    gl.disableVertexAttribArray(vertexPositionLocation);

    gl.bindTexture(gl.TEXTURE_2D, null);
}
</script>
<script>
window.onload = function() {
    init();

    $("#peekaboo").resizable({ minWidth: 500 });
    $("#peekaboo").show();

    var editor = ace.edit("editor");
    editor.setTheme("ace/theme/clouds_midnight");

    var Mode = require("ace/mode/glsl").Mode;
    editor.getSession().setMode(new Mode());
    editor.getSession().setUseWrapMode(true);
    editor.getSession().setOption("firstLineNumber", 0);
    editor.setShowPrintMargin(false);

    editor.renderer.setHScrollBarAlwaysVisible(false);
    editor.renderer.setVScrollBarAlwaysVisible(false);

    var markers = [];

    var nb_sliders = 0;

    var timeout = null;

    $("#peekaboo").bind("resize", function(event, ui) {
        editor.resize();
    });

    $("#peekaboo").css("height", window.innerHeight);

    $("#stuff").show();

    var rebuild = function() {
        var vertex_shader, fragment_shader;
        var new_prog;
        var text = editor.getSession().getValue();

        vertex_shader   = g_vshader;
        fragment_shader = text;

        editor.getSession().clearAnnotations();
        for (m in markers) {
            editor.getSession().removeMarker(markers[m]);
        }
        markers = [];

        new_prog = createProgram(vertex_shader, fragment_shader);

        var status_color = new_prog ? "#404040" : "#7f0000"
        $("#status").css("border-color", status_color);

        marked_lines = [];

        var text = "Compiled successfully!";

        if (new_prog !== null) {
            currentProgram = new_prog;
        }
        else if (gl_shader_error !== null) {
            gl_shader_error = gl_shader_error.replace(/\0/g, "");
            text = gl_shader_error.replace(/\n/g, "<br>");

            var annos = [];

            var re = /\d+:(\d+):(.*)\n/g;
            var match;
            while (match = re.exec(gl_shader_error)) {
                var line = parseInt(match[1]);
                var error = match[2];

                if (marked_lines[line]) {
                    error = " **** " + error;
                }

                annos.push({
                    row: line,
                    column: 0,
                    text: error,
                    type: "error"
                });

                if (marked_lines[line] === undefined) {
                    marked_lines[line] = true;

                    var Range = require("ace/range").Range;
                    var line_range = new Range(line, 0, line + 1, 0);

                    var marker = editor.getSession().addMarker(line_range, "ace_error", "line", false);
                    markers.push(marker);
                }
            }

            editor.getSession().setAnnotations(annos);
        }

        $("#status_text").html(text);

        g_sample_count = 0;
    }

    $("#editor").keyup(function() {
        clearTimeout(timeout);
        timeout = setTimeout(rebuild, 500);
    });
}
</script>