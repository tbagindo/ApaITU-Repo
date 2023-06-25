static const char MAIN_PAGE[] PROGMEM = R"==(
<!DOCTYPE html>
<html>
  <body>
    <h1>NodeMCU WiFi & WebServer</h1>
    <table>
      <tr>
        <td>Distance</td>
        <td>:</td> 
        <td><span id='waterLevel'></span></td>
        <td> cm</td>
      </tr>
      <tr>
        <td>Water Level</td>
        <td>:</td> 
        <td><span id='distance'></span></td>
        <td> cm</td>
      </tr>
      <tr>
        <td>Tank Volume</td>
        <td>:</td> 
        <td><span id='volume'></span></td>
        <td> L</td>
      </tr>
      <tr>
        <td>RelayState1</td>
        <td>:</td> 
        <td><span id='relay1'></span></td>
        <td><button id='r1btn' onclick="btnClick('relay1','r1btn','1')">ON</button></td>
      </tr>
      <tr>
        <td>RelayState2</td>
        <td>:</td> 
        <td><span id='relay2'></span></td>
        <td><button id='r2btn' onclick="btnClick('relay2','r2btn','2')">OFF</button></td>
      </tr>
      <tr>
        <td>Pump Mode</td>
        <td>:</td> 
        <td><span id='mode'></span></td>
        <td><button id='mbtn' onclick="modeClick()">Automatic</button></td>
      </tr>  
  </body>
  <script>
    var Socket;
    function init(){
      Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
      Socket.onmessage = function(event){
        processCommand(event);
      };
    }
    function processCommand(event){
      const r1 = document.getElementById('relay1');
      const r2 = document.getElementById('relay2')
      var obj = JSON.parse(event.data);
      document.getElementById('distance').innerHTML = obj.wDistance;
      document.getElementById('waterLevel').innerHTML = obj.waterLevel;
      document.getElementById('volume').innerHTML = obj.volume;
      if(obj.relay1state =='0'){
        r1.innerHTML = "OFF";
      }else{
        r1.innerHTML = "ON";
      }
      if(obj.relay2state =='0'){
        r2.innerHTML = "OFF";
      }else{
        r2.innerHTML = "ON";
      }
      if(obj.pMode == '1'){
        document.getElementById('mode').innerHTML = 'Automatic';
      }else{
        document.getElementById('mode').innerHTML = 'Manual';
      }
      
      console.log(obj.waterLevel);
      console.log(obj.volume);
    }
    function modeClick(){
      const mobj = document.getElementById('mode');
      const b1 = document.getElementById('mbtn');
      var ch_id = 3;
      var ch_data;
      if(mobj.innerHTML == 'Automatic'){
          ch_data = 0;
          b1.textContent = "Automatic";
          mobj.innerHTML = "Manual";
      }else{
          ch_data = 1;
          b1.textContent = "Manual";
          mobj.innerHTML = "Automatic";
      }  

      var msg = {
        ch_id: ch_id,
        ch_data: ch_data
      };
      Socket.send(JSON.stringify(msg));
    }
    function btnClick(relay,relaybutton,cid){
      var r1 = document.getElementById(relay);
      const b1 = document.getElementById(relaybutton);
      var ch_id = cid;
      var ch_data;
      if(r1.innerHTML == 'ON'){
          ch_data = 0;
          b1.textContent = "ON";
          r1.innerHTML = "OFF";
      }else{
          ch_data = 1;
          b1.textContent = "OFF";
          r1.innerHTML = "ON";
      }  

      var msg = {
        ch_id: ch_id,
        ch_data: ch_data
      };
      Socket.send(JSON.stringify(msg));
    }
    window.onload = function(event) {
    init();
    }
  </script>
</html>
)==";
