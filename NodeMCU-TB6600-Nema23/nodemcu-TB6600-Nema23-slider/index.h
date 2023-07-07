static const char MAIN_PAGE[] PROGMEM = R"==(
<!DOCTYPE html>
<html>
  <body>
    <h2>NodeMCU TB6600 Nema 23 Stepper Motor</h2>
    <table>
      <tr>
        <td width="120px">Steps</td>
        <td width="20px">:</td> 
        <td width="140px"><input type="number" id="step" name="step" min="100" max="99999" step="100" value="800"></td>
        <td width="30px"></td>
        <td width="100px"></td>
      </tr>
      <tr>
        <td>Revolutions</td>
        <td>:</td> 
        <td><input type="range" id="revs" name="revs" min="1" max="20" step="1" value"1" onchange="updateRevs(this.value,'sRevs')"></td>
        <td align="right"><span id="sRevs">1</span></td>
        <td>Revs</td>
      </tr>
      <tr>
        <td>Direction</td>
        <td>:</td> 
        <td><input type="radio" class="dir" name="dir" value="cw" checked> CW <input type="radio" class="dir" name="dir" value="ccw"> CCW </td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td>Speed</td>
        <td>:</td> 
        <td><input type="range" id="speed" name="speed" min="400"  max="3000" step="100" value"1000" onchange="updateRange(this.value,'sSpeed')"></td>
        <td><span id="sSpeed"></span></td>
        <td>Steps/s</td>
      </tr> 
      <tr>
        <td>Acceleration</td>
        <td>:</td> 
        <td><input type="range" id="accel" name="accel" min="400" max="2000" step="100" value"1000" onchange="updateRange(this.value,'sAccel')"></td>
        <td><span id="sAccel"></span></td>
        <td>Steps/s"</td>
      </tr>
      <tr>
        <td>Actual Speed</td>
        <td>:</td> 
        <td><span id="aSpeed"></span></td>
        <td></td>
        <td>Steps/s</td>
      </tr>
      <tr>
        <td>Current Position</td>
        <td>:</td> 
        <td><span id="cPos"></span></td>
        <td></td>
        <td>steps</td>
      </tr>
      <tr>
        <td>last Position</td>
        <td>:</td> 
        <td><span id="lPos"></span></td>
        <td></td>
        <td>steps</td>
      </tr>
      <tr>
        <td>Actual Position</td>
        <td>:</td> 
        <td><span id="aPos"></span></td>
        <td></td>
        <td>steps</td>
      </tr>
      <tr>
        <td>Motor</td>
        <td>:</td> 
        <td><span id='mState'></span></td>
        <td></td>
        <td><button id='btnMotor' onclick="btnClick('mState')" style="background-color: lightgreen; height: 25px; width: 100px"> - </button></td>
      </tr> 
      <tr>
        <td>Bounce</td>
        <td>:</td> 
        <td><span id='bState'></span></td>
        <td></td>
        <td><button id='btnBounce' onclick="bounceClick('mState')" style="background-color: lightgreen; height: 25px; width: 100px"> - </button></td>
      </tr> 
  </body>
  <script>
    document.getElementById("step").defaultValue = 800;
    document.getElementById("revs").defaultValue = 1;
    document.getElementById("sRevs").innerHTML = 1;
    document.getElementById("speed").defaultValue = 400;
    document.getElementById("sSpeed").innerHTML = 400;
    document.getElementById("accel").defaultValue = 400;
    document.getElementById("sAccel").innerHTML = 400;
        
    var Socket;
    function init(){
      Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
      Socket.onmessage = function(event){
        processerialHandler(event);
      };
    }
    function processerialHandler(event){
      const aSpeed = document.getElementById("aSpeed");
      const cPos = document.getElementById("cPos");
      const aPos = document.getElementById("aPos");
      const mState = document.getElementById("mState");
      const btnMotor = document.getElementById("btnMotor");
      const btnBounce = document.getElementById("btnBounce");
      var obj = JSON.parse(event.data);
      aSpeed.innerHTML = obj.aSpeed;
      aPos.innerHTML = obj.aPos;
      lPos.innerHTML = obj.lPos;      
      cPos.innerHTML = obj.cPos;
      if(obj.mState =="0"){
        btnMotor.textContent = "RUN";
        mState.innerHTML = "Stop";
      }else{
        btnMotor.textContent = "Stop";
        mState.innerHTML = "Running";
      }
      if(obj.bState =="0"){
        btnBounce.textContent = "Bouncing";
        document.getElementById("bState").innerHTML = "-";
      }else{
        btnBounce.textContent = "Stop";
        document.getElementById("bState").innerHTML = "Bouncing";
      }

    }
    function updateRange(value,sid){
      let spanValue = document.getElementById(sid);
      spanValue.innerHTML = value;
    }
    function updateRevs(value,sid){
      let spanValue = document.getElementById(sid);
      spanValue.innerHTML = value;
      document.getElementById("step").value = value * 800;
    }
    function bounceClick(state){
      var step = document.getElementById("step");
      var speed = document.getElementById("speed");
      var accel = document.getElementById("accel");
      var mState = document.getElementById("mState");
      var dir;
      
    }
    function btnClick(state){
      console.log(state);
      var step = document.getElementById("step");
      var speed = document.getElementById("speed");
      var accel = document.getElementById("accel");
      var mState = document.getElementById("mState");
      var dir;
      let radioBtns = document.querySelectorAll("input[type='radio'].dir"); 
      radioBtns.forEach(function(e){
        if(e.checked){
          if(e.value == "cw"){
            dir = 0;
          }else{
            dir = 1;
          }
        }
      });
      var msg;
      if(mState.innerHTML == "Stop"){ 
        msg = {
          step : step.value,
          speed : speed.value,
          accel : accel.value,
          dir : dir,
          mState : 1,
          bState : 0       
        };
      }else{
        msg = {
          step : 0,
          speed : 0,
          accel : 0,
          dir : 0,
          mState : 0,
          bState : 0   
        };
      }
      Socket.send(JSON.stringify(msg));
    }

    function bounceClick(state){
      console.log(state);
      var step = document.getElementById("step");
      var speed = document.getElementById("speed");
      var accel = document.getElementById("accel");
      var mState = document.getElementById("mState");
      var bState = document.getElementById("bState");
      var msg;
      if(mState.innerHTML == "Stop" && bState.innerHTML == "-"){ 
        msg = {
          step : step.value,
          speed : speed.value,
          accel : accel.value,
          dir : 2,
          mState : 1,
          bState : 1         
        };
      }else{
        msg = {
          step : 0,
          speed : 0,
          accel : 0,
          dir : 0,
          mState : 0,
          bState : 0   
        };
      }
      Socket.send(JSON.stringify(msg));
    }
    window.onload = function(event) {
    init();
    }
  </script>
</html>
)==";
