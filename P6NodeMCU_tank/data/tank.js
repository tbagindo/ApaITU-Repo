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