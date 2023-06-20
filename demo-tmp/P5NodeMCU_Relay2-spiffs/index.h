static const char MAIN_PAGE[] PROGMEM = R"==(
<!DOCTYPE html>
<html>
  <head>
    <style>
      .tk {
        position:relative;
        width:40%; 
        height:120px;
        padding-top:40px;
        margin: 0 auto;
        background:rgba(255,200,255,0.8);
        border-radius: 100%/40px;
        border : 2px solid blue;
        text-align:center;
        z-index:1;
        overflow:hidden;
      }

      .lq {
        position: absolute;
        background:rgba(0,0,255,0.99);
        width: 100%;
        height:0;
        bottom: 0;
        border-radius:100%/40px;
        border-bottom:1px solid #000;
      }

      .tk:after{
        position: absolute;
        top: -10px;
        left:0px;
        width: 100%;
        height:26%;
        content: '';
        border-radius:100%;
        border:2px solid blue;    
      }

      .ring {
        position: absolute;
        border-radius:100%;
        top: 0;

        width: 98%;
        height:40%;
        content: '';
        border:2px solid white;
      }

      .text {
        display: block;
        color : white;
        position:absolute;
        top: 45%;
        left: 45%;
        z-index: 1;
      }
      body{
        margin :0px;
        padding : 0px;
        box-sizing : border-box;
        font-family : arial;
        text-align: center;
      }
      button{
        background-color: lightgray;
        width : 100%;
      }
    </style>
  </head>
  <body>
    <h3>NodeMCU WiFi & WebServer</h3><br>
    <div class="tk">

      <div id="level" class="lq" style="height : 70%">
        <div id="ring" class="ring" style="height : 40%"></div>
      </div>
      <span id="percent" class="text">70%</span>
    </div>

    <table width="40%" align="center">
      <tr>
        <td width="40%">Distance</td>
        <td width="10%">:</td> 
        <td width="20%" align="right"><span id='distance'></span></td>
        <td width="30%">cm</td>
      </tr>         <tr>
      <td>Water Level</td>
        <td>:</td> 
        <td><span id='waterLevel'></td>
        <td>cm</td>
      </tr>
      <tr>
        <td>Tank Volume</td>
        <td>:</td> 
        <td><span id='volume'></span></td>
        <td>L</td>
      </tr>
      <tr>
        <td>RelayState1</td>
        <td>:</td> 
        <td><span id='relay1'></span></td>
        <td><button id="r1btn" onclick="btnClick('1')">Relay1</button></td>
      </tr>
      <tr>
        <td>RelayState2</td>
        <td>:</td> 
        <td><span id='relay2'></span></td>
        <td><button id="r1btn" onclick="btnClick('2')">Relay 2</button></td>
      </tr>
      <tr>
        <td>Pump Mode</td>
        <td>:</td> 
        <td><span id='mode'></span></td>
        <td><button id="mbtn" onclick="btnClick('3')">Mode</button></td>
      </tr>
    </table>
    <input type="range" id="rangeLevel" min="0" max="100" step="2" onchange="drawTank(this.value)">
  </body>
  <script>
    var Socket;
    function init(){
      Socket = new WebSocket("ws://" + window.location.hostname + ":81/");
      Socket.onmessage = function(event){
        processCommand(event);
      };
    }
    function processCommand(event){
    var r1;
    var r2;
    var mode;
    var percent = document.getElementById('percent').innerHTML;
    var obj = JSON.parse(event.data);
    document.getElementById('distance').innerHTML = obj.wDistance;
    document.getElementById('waterLevel').innerHTML = obj.waterLevel;
    document.getElementById('volume').innerHTML = obj.volume;
    percent = parseInt(obj.percent); 
      
    // states and mode
    if(obj.relay1state == '1')r1 = "ON";
      else r1 = "OFF";
    if(obj.relay2state == '1')r2 = "ON";
      else r2 = "OFF";
    if(obj.pMode == '1')mode = "Auto";
      else mode = "Manual";
    document.getElementById('relay1').innerHTML = r1;
    document.getElementById('relay2').innerHTML = r2;
    document.getElementById('mode').innerHTML = mode;
    
    //draw tank
    drawTank(percent);
    
    }
    window.onload = function(event) {
    init();
    }
    function drawTank(level){
      console.log("drawTank(" + level + ") in %");
      var Percent = document.getElementById('percent');
      var TankLevel = document.getElementById('level'); 
      var RingLevel = document.getElementById('ring');
      
      var ring;
      if(level > 86){
        ring = 100 - level + 20 + "%";
      }else{
        ring = 100 - level + 10 + "%";
      }

      TankLevel.style.height = level + "%";
      RingLevel.style.height = ring;
      Percent.innerHTML = level + "%";
    }
    
    function btnClick(did){
      //DataID Relay1 : 1,  Relay2 : 2, mode : 3
      //value  ON : 1, OFF : 0
    //
    var id = parseInt(did);
    const stateID = ["0","relay1","relay2","mode"];
      var value;  
      var state = document.getElementById(stateID[did]).innerHTML;
      if(did == 1 || did == 2){ 
        if(state == "ON"){
          value = 0;
          state = "OFF";
        }else{ 
          value = 1;
          state = "ON"
        }
      }
      if(did == 3){ 
        if(state == "Auto"){
          value = 0;
          state = "Manual";
        }else{ 
          value = 1;
          state = "Auto"
        }
      }
      //document.getElementById(stateID[did]).innerHTML = state;

      var msg = { 
        did: did,
        value: value
      };
      Socket.send(JSON.stringify(msg));

      
    }
    
    //
  </script>
</html>
)==";
