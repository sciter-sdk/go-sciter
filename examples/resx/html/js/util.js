window.util = {};

util.openDialog = function (url, width = 800, height = 600, data = null) {
    return Window.this.modal({
        url: url,
        parent: Window.this,
        type: Window.FRAME_WINDOW,
        width: width,
        height: height,
        alignment: -8,
        parameters: data
    });
}

//获取随机数
util.randomString = function (len) {
    len = len || 32;
    let chars = 'ABCDEFGHJKMNPQRSTWXYZabcdefhijkmnprstwxyz2345678';
    let maxPos = chars.length;
    let pwd = '';
    for (let i = 0; i < len; i++) {
        pwd += chars.charAt(Math.floor(Math.random() * maxPos));
    }
    return pwd;
}

util.moveCenter = function (width = 0, height = 0) {
    let view = Window.this;
    let [width1, height1] = view.box('dimension', 'border');
    let [width2, height2] = view.box('dimension', 'client');
    if (width == 0) {
        width = width2;
    }
    if (height == 0) {
        height = height2;
    }
    let nw = width + (width1 - width2);
    let nh = height + (height1 - height2);
    let [sw, sh] = view.screenBox('frame', 'dimension');
    let left = (sw - nw) / 2;
    let top = (sh - nh) / 2;
    view.move(left, top, width, height, "client");
}

util.ready = function (fn) {
    let timer = setInterval(function () {
        clearInterval(timer)
        fn(Window.this)
    }, 10)
}