    var Socket;
    function init(){
      Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
      Socket.onmessage = function(event){
        processCommand(event);
      };
    }
    function processCommand(event){
      const r1 = document.getElementById('relay1');
      const r2 = document.getElementById('relay2');
      const b1 = document.getElementById('r1btn');
      const b2 = document.getElementById('r2btn');
      var obj = JSON.parse(event.data);
      document.getElementById('rawDist').innerHTML = obj.rawDist;
      document.getElementById('distance').innerHTML = obj.wDistance;
      document.getElementById('waterLevel').innerHTML = obj.waterLevel;
      document.getElementById('volume').innerHTML = obj.volume;
      document.getElementById('percentage').innerHTML = obj.percent;
      if(obj.relay1state =='0'){
        b1.textContent = "ON";
        r1.innerHTML = "OFF";
      }else{
        b1.textContent = "OFF";
        r1.innerHTML = "ON";
      }
      if(obj.relay2state =='0'){
        b2.textContent = "ON";
        r2.innerHTML = "OFF";
      }else{
        b2.textContent = "OFF";
        r2.innerHTML = "ON";
      }
      if(obj.pMode == '1'){
        document.getElementById('mode').innerHTML = 'Automatic';
      }else{
        document.getElementById('mode').innerHTML = 'Manual';
      }
	  drawTank(obj.percent);
      
      console.log(obj.waterLevel);
      console.log(obj.volume);
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
			if(level<50){
				Percent.style.color = "blue";
			}else{
				Percent.style.color = "white";
			}

			TankLevel.style.height = level + "%";
			RingLevel.style.height = ring;
			Percent.innerHTML = level + "%";
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

    "btnClick('relay2','r2btn','2')"
    function btnClick(relay,relaybutton,cid){
      var r1 = document.getElementById(relay);
      const b1 = document.getElementById(relaybutton);
      var ch_id = cid;
      var ch_data;
      if(r1.innerHTML == 'ON'){
          ch_data = 0;
      }else{
          ch_data = 1;
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