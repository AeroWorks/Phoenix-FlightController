var connectionId = -1;

var graph_gyro;
var graph_accel;
var graph_kinematics;

$(document).ready(function() {
    var port_picker = $('div#port-picker #port');
    var baud_picker = $('div#port-picker #baud');
    
    $('div#port-picker a.refresh').click(function() {
        port_picker.html('');

        chrome.serial.getPorts(function(ports) {
            ports.forEach(function(port) {
                port_picker.append($("<option/>", {
                    value: port,
                    text: port
                }));        
            });
        });

        console.log("Available port list requested.");
    });
    // software click to refresh port picker select during first load
    $('div#port-picker a.refresh').click();
    
    $('div#port-picker a.connect').toggle(function() {        
        var selected_port = port_picker.val();
        var selected_baud = parseInt(baud_picker.val());
        
        chrome.serial.open(selected_port, {
            bitrate: selected_baud
        }, onOpen);
        
        $(this).text('[ Disconnect ]');
    }, function() {
        chrome.serial.close(connectionId, onClosed); // Seems to be broken
        
        $(this).text('[ Connect ]');
    });
    
    // Setup Graphs
    graph_gyro = new Rickshaw.Graph( {
        element: document.getElementById("graph_gyro"),
        width: 1278,
        height: 200,
        renderer: 'line',
        max: 20,
        series: new Rickshaw.Series.FixedDuration([{ name: 'one' }], undefined, {
            timeInterval: 10,
            maxDataPoints: 1000,
            timeBase: new Date().getTime()
        }) 
    } );

    graph_gyro.render();   

    graph_accel = new Rickshaw.Graph( {
        element: document.getElementById("graph_accel"),
        width: 1278,
        height: 200,
        renderer: 'line',
        max: 3,
        series: new Rickshaw.Series.FixedDuration([{ name: 'one' }], undefined, {
            timeInterval: 10,
            maxDataPoints: 1000,
            timeBase: new Date().getTime()
        }) 
    } );

    graph_gyro.render();       
    
    graph_kinematics = new Rickshaw.Graph( {
        element: document.getElementById("graph_kinematics"),
        width: 1278,
        height: 200,
        renderer: 'line',
        max: 360,
        series: new Rickshaw.Series.FixedDuration([{ name: 'one' }], undefined, {
            timeInterval: 10,
            maxDataPoints: 1000,
            timeBase: new Date().getTime()
        }) 
    } );

    graph_kinematics.render();
    
});

function onOpen(openInfo) {
    connectionId = openInfo.connectionId;
    
    if (connectionId != -1) {
        console.log('Connection was opened with ID: ' + connectionId);
        
        // Start reading
        chrome.serial.read(connectionId, 1000, onCharRead);
    } else {
        console.log('There was a problem in opening the connection.');
    }    
};

function onClosed(result) {
    if (result) console.log('Connection closed successfully.');
    else console.log('There was an error that happened during "connection-close" procedure.');
};

function onCharRead(readInfo) {
    if (readInfo && readInfo.bytesRead > 0 && readInfo.data) {
        var str = ab2str(readInfo.data);
        if (str[readInfo.bytesRead-1]==='\n') {
            str = str.substring(0, readInfo.bytesRead-1);
        }
        
        // Line read
        // str = standard string
        // ar = str split into array
        var ar = str.split(",");
        
        // Gyro
        ar[0] = parseFloat(ar[0]); // X
        ar[1] = parseFloat(ar[1]); // Y
        ar[2] = parseFloat(ar[2]); // Z
        
        var data_gyro = {one: ar[0], two: ar[1], three: ar[2]};
        graph_gyro.series.addData(data_gyro);
        graph_gyro.render();        
        
        // Accel
        ar[3] = parseFloat(ar[3]); // X
        ar[4] = parseFloat(ar[4]); // Y
        ar[5] = parseFloat(ar[5]); // Z

        var data_accel = {one: ar[3], two: ar[4], three: ar[5]};
        graph_accel.series.addData(data_accel);
        graph_accel.render();          
        
        // Kinematics
        ar[6] = parseFloat(ar[6]); // Roll
        ar[7] = parseFloat(ar[7]); // Pitch
        ar[8] = parseFloat(ar[8]); // Yaw  

        var data_kinematics = {one: ar[6], two: ar[7], three: ar[8]};
        graph_kinematics.series.addData(data_kinematics);
        graph_kinematics.render();
        
        // TX
        ar[9] = parseInt(ar[9]); // TX Roll
        ar[10] = parseInt(ar[10]); // TX Pitch
        ar[11] = parseInt(ar[11]); // TX Throttle
        ar[12] = parseInt(ar[12]); // TX Yaw
        
    }
    chrome.serial.read(connectionId, 1000, onCharRead);
}

var ab2str = function(buf) {
    return String.fromCharCode.apply(null, new Uint8Array(buf));
};