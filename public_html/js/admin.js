//Admin Hide button functionality
function hideAdminMenus(){
	$("#GameBrowse").hide();
	$("#AddGame").hide();
	$("#ModGame").hide();
	$("#UserManage").hide();
	$("#Timeout").hide();
	$("#RegRequests").hide();
	$("#History").hide();
	$("#ModGame").hide();
}
function onGameBrowseClick(){
	onAdminLoad();
}
function onGameAddClick(){
	hideAdminMenus();
	$("#AddGame").show();
}
function onGameModClick(){
	hideAdminMenus();
	$("#ModGame").show();
}
function onUserManageClick(){
	hideAdminMenus();
	$("#UserManage").show();
}
function onTimeoutClick(){
	hideAdminMenus();
	$("#Timeout").show();
}
function updateRegReqDisplay(destElem, data){
	while (iMax > 1){
		destElem.removeChild(destElem.lastChild);
		iMax--;
	}
	
	for (var i=0;i<rCount;i++){
		var newRow = document.createElement("TR"),
			emailCol = document.createElement("TD"),
			fNameCol = document.createElement("TD"),
			lNameCol = document.createElement("TD"),
			sentCol = document.createElement("TD"),
			buttonCol = document.createElement("TD");
		
		newRow.appendChild(emailCol);
		newRow.appendChild(fNameCol);
		newRow.appendChild(lNameCol);
		newRow.appendChild(sentCol);
		newRow.appendChild(buttonCol);
		
		emailCol.innerHTML = data[1+(i*5)+4];
		fNameCol.innerHTML = data[1+(i*5)+2];
		lNameCol.innerHTML = data[1+(i*5)+3];
		if (data[1+(i*5)+5] == "-1"){
			sentCol.innerHTML = "<input type=\"checkbox\" disabled></input>";
		} else {
			sentCol.innerHTML = "<input type=\"checkbox\" disabled checked></input>";
		}
		buttonCol.innerHTML = "<input type=\"button\" value=\"Send Invite\" data-regid=\"" + data[1+(i*5)] + "\" onclick=\"onRegSendClick(event);\"></input> <input type=\"button\" value=\"Delete\" data-regid=\"" + data[1+(i*5)] + "\" onclick=\"onRegDeleteClick(event);\"></input>";
		destElem.appendChild(newRow);
	}
}
function onRegReqClick(){
	hideAdminMenus();
	$.ajax("cgi-bin/admin.cgi", {data: getCmdMsg(ADMIN_REGLIST), type: "POST", success: function(data){
		data = splitData(data);
		var rCount = parseInt(data[1]),
			rowHTML = "",
			destElem = getElem("regRequestsTbl"),
			iMax = destElem.childNodes.length - 1;
			
		updateRegReqDisplay(destElem, data);

		$("#RegRequests").fadeIn(300);
	}});
}
function onRegSendClick(event){
	var id = event.target.dataset.Regid;
	$.ajax("cgi-bin/admin.cgi", {data: getCmdMsg(ADMIN_REGSEND) + DLM + id, type: "POST", success: function(data){
		data = splitData(data);
		var rCount = parseInt(data[1]),
			rowHTML = "",
			destElem = getElem("regRequestsTbl"),
			iMax = destElem.childNodes.length - 1;
			
		updateRegReqDisplay(destElem, data);
	}});
}
function onRegDeleteClick(event){
	var id = event.target.dataset.Regid;
	$.ajax("cgi-bin/admin.cgi", {data: getCmdMsg(ADMIN_REGDELETE) + DLM + id, type: "POST", success: function(data){
		data = splitData(data);
		var rCount = parseInt(data[1]),
			rowHTML = "",
			destElem = getElem("regRequestsTbl"),
			iMax = destElem.childNodes.length - 1;
			
		updateRegReqDisplay(destElem, data);
	}});
}
function onHistoryClick(){
	hideAdminMenus();
	$.ajax("cgi-bin/admin.cgi", {data: getCmdMsg(ADMIN_LOG), type: "POST", success: function(data){
		data = splitData(data);
		var rCount = parseInt(data[1]),
			rowHTML = "";
		for (var i=0;i<rCount;i++){
			rowHTML += "<div>" + data[2+i] + "</div>";
		}
		$("#logRows").html(rowHTML);
		$("#History").fadeIn(300);
	}});
}
function openModifyGame(){
	var gameName = $("gamesList").val();
	var dList = document.getElementById("games");
	var dItem = dList.options[dList.selectedIndex];
	
	if (typeof dItem == "undefined") return;
	
	$("#mod_ID").val(dItem.dataset.gameId);
	$("#mod_gamesListGM").val(dItem.dataset.gameName);
	$("#mod_GameURL").val(dItem.dataset.gameUrl);
	$("#mod_Desc").val(dItem.dataset.gameDesc);
	$("#mod_MinUsers").val(dItem.dataset.gameUsers);
	$("#mod_MaxUsers").val(dItem.dataset.gameUsers);
	$("#ModGame").fadeIn(300);
}
function deleteModifyGame(){
	var result = confirm("You are about to delete a game, press cancel to stop this action.");
	if (result == true){
		var gameName = $("gamesList").val();
		var dList = document.getElementById("games");
		var dItem = dList.options[dList.selectedIndex];
		
		if (typeof dItem == "undefined") return;
		
		var delID = dItem.dataset.gameId;
		$.ajax("cgi-bin/games.cgi", {data: getCmdMsg(GAMES_REMOVE) + DLM + delID, 
			type: "POST", 
			success: function(data){
				data = splitData(data);
				if (data[0] == "ERROR"){
					return;
				} else if (data[0] == "REMOVE"){
					var remID = data[1],
						gameName = $("gamesList").val(),
						dList = document.getElementById("games");
					
					for (var i=0; i<dList.length; i++){
						var dLID = dList.options[i].dataset.gameId;
						if (dList.options[i].dataset.gameId == remID){
							dList.options[i].remove(i);
							return;
						}
					}
				}
			}
		});
		$("#ModGame").hide();
		$("#ModGame").fadeOut(0);
	}
}
function gameSendModifyClick(){
	var gameID = $("#mod_ID").val(),
		gameName = $("#mod_gamesListGM").val(),
		gameUrl = $("#mod_GameURL").val(),
		gameDesc = $("#mod_Desc").val(),
		gameUsers = $("#mod_MinUsers").val(),
		gameUsers2 = $("#mod_MaxUsers").val();
		
	$.ajax("cgi-bin/games.cgi", {data: getCmdMsg(GAMES_SAVEGAME, [gameID, gameName, gameUrl, gameDesc, gameUsers, gameUsers2]), 
		type: "POST", 
		success: function(data){
			data = splitData(data);
			if (data[0] == "ERROR"){
				//** TODO: Show error messages
			} else {
				$("#ModGame").hide();
			}
	}});
}
function gameSendAddClick(){
	var gameName = $("#add_gamesListGM").val(),
		gameUrl = $("#add_GameURL").val(),
		gameDesc = $("#add_Desc").val(),
		gameUsers = $("#add_MinUsers").val();
		
	$.ajax("cgi-bin/games.cgi", {data: sesKey + DLM + GAMES_ADDGAME + DLM + gameName + DLM + gameUsers + DLM + gameUsers + DLM + gameUrl + DLM + gameDesc, 
		type: "POST", 
		success: function(data){
		
	}});
	$("#ModGame").hide();
}



function onAdminLoad(){
	hideAdminMenus();
	$.ajax("cgi-bin/games.cgi", {data: sesKey + DLM + GAMES_GAMES, type: "POST", success: function(data){
		data = splitData(data);
		
		var htmlOptions = "";
		var gameCount = parseInt(data[1]);
				
		for (var i = 0; i < gameCount; i++){
			htmlOptions += "<option";
			htmlOptions += " data-game-Id='" + data[2 + (i * 5)] + "'";
			htmlOptions += " data-game-Name='" + data[2 + (i * 5) + 1] + "'";
			htmlOptions += " data-game-Url='" + data[2 + (i * 5) + 2] + "'";
			htmlOptions += " data-game-Desc='" + data[2 + (i * 5) + 3] + "'";
			htmlOptions += " data-game-Users='" + data[2 + (i * 5) + 4] + "'";
			htmlOptions += ">" + data[2 + (i * 5) + 1] + "</option>";	
		}
		$("#games").html(htmlOptions);
	
		$("#GameBrowse").show();
		$("#innerBody").fadeIn(fadeInSpeed);
	}, error: function(data){
		data = splitData(data);
		$("#innerBody").fadeIn(fadeInSpeed);
	}});
}
function onAdminClick(){
	$("#innerBody").fadeOut(fadeOutSpeed, function(){$("#innerBody").load("admin.html", "", function(){
		onAdminLoad();
	})});
}