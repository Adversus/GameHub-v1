//** Editted by Donald Franklin

//********** GLOBAL VARIABLES **********//
var cgiUrl = "cgi-bin/";
var sesKey = "";
var UserID = 0,
	UserName = "Anonymous",
	UserLevel = 0,
	MuteLevel = 0,
	BanLevel = 0,
	fadeOutSpeed = 600,
	fadeInSpeed = 400,
	defaultPlayFile = "play.php",
	pauseChat = false;
var uLevelNames = ['NonUser', 'Player', 'Poster', 'Moderator', 'Admin'];
var DLM = "_|"; //** Delimiting value
var timeObj = new Date();
var lastUpdate = Math.floor(timeObj.getTime() / 1000)-10,
	lastReadyUpdate = Math.floor(timeObj.getTime() / 1000)-10,
	chatInstances = 0,
	readyInstances = 0;
var updateWait = 20 * 1000,	//(20 seconds * 1000 milliseconds)
	readyWait = 30 * 1000;	//(30 seconds * 1000 milliseconds)
var pageTarget = "",
	nextURL = "";
	
//********** SERVER COMMANDS **********//
//** These are commands the client sends and the server responds with
var MAIN_GAMELIST = "GAMES",				//Get list of games +(optional: sort by)
	MAIN_SESSIONLIST = "SESSIONS",			//Get list of sessions +(game id)
	MAIN_POSTGAME = "POST", 				//Start game session +(game id, session name?, password, number of players)
	MAIN_GAMEUPDATE = "UPDATE",				//Get status of game session +(game session id)
	MAIN_CHANGEUSER = "CHANGEUSER",			//Poster sets position +(new user position)
	MAIN_STARTGAME = "START",				//Sent when everything is ready so the server knows to delete the session
	MAIN_JOIN = "JOIN",						//Join game session +(game id)
	MAIN_REGREQUEST = "REGREQUEST";			//Send a registration request +(fname, lname, email)
	
var PLAY_UPDATE = "UPDATE",
	PLAY_QUIT = "QUIT",
	PLAY_START = "START";
	
var PROFILE_MYPROFILE = "SHOWME",			//Get info for user's profile
	PROFILE_OTHERPROFILE = "SHOWOTHER",		//Get info for someone elses profile +(profile user id)
	PROFILE_COLOR = "COLOR",				//Set the user's profile color +(string of length 6)
	PROFILE_NEWREG = "NEWREG",				//Randomizes user's registration code
	PROFILE_NEWPASS = "NEWPASS",			//Change user's password +(OldPassword, NewPassword)
	PROFILE_NEWSTATUS = "NEWSTATS";			//Change user's stats +(FirstName, LastName, Email)
	
var ADMIN_USERS = "USERS",					//Get list of users +(optional: name)
	ADMIN_TIMEOUT = "TIMEOUT",				//Set new global timeout +(new value)
	ADMIN_USERLEVEL = "USERLEVEL",			//Set user's level +(user id, new userlevel)
	ADMIN_REGLIST = "REGLIST",				//Get list of registration requests
	ADMIN_BANLIST = "BANLIST",				//Get list of banned or muted users
	ADMIN_BANUSER = "SETBAN",				//Set user ban level +(user id)
	ADMIN_MUTEUSER = "SETMUTE",				//Set user mute level +(user id)
	ADMIN_USERDELETE = "USERDELETE",		//Remove user's account +(user id)
	ADMIN_LOG = "LOG"						//Get list of recent commands
	
var GAMES_MYGAME = "MYGAMES",				//Get list of users games
	GAMES_ADDGAME = "ADDGAME",				//Add a new game +(GameName, Description, URL, MinPlayers, MaxPlayers)
	GAMES_GAMES = "GAMES",					//Get a list of top 15 games. +(sorting: (empty), "RATING", "TIME", "NAME")
	GAMES_REMOVE = "REMOVE",				//Drop game from table +(GameId)
	GAMES_SAVEGAME = "SAVEGAME",			//Save game data
	GAMES_POST = "POST";					//Post a game session

//** Commands that the client receives sometimes instead of the above
var OTHER_TIMEOUT = "TIMEOUT",				//If the user's session timed out this is the only command they get
	OTHER_ERROR = "ERROR";					//This is always the first thing returned if there's an error
	
	

//***************************************************//
//*					UTILITY METHODS					*//
//***************************************************//
function getElem(elemID){
	return document.getElementById(elemID);
}
function splitData(str){
	var outputArr = [];
	var pos = str.indexOf(DLM),
		esc = "\|",
		startPos = 0,
		tmpStr;
	
	//** Handle newline char
	var tmp = str.slice(str.length-1, str.length);
	if (str.slice(str.length-1, str.length) == "\n"){
		str = str.slice(0, str.length-1);
	}
	
	//** Split string and place chunks into an array
	while (pos != -1){
		tmpStr = str.substring(startPos, pos);

		//** Strip escape characters
		tmpStr.replace("\\|", "|");

		//** Add to array
		outputArr.push(tmpStr);
		
		//** Update positions for next iteration
		startPos = pos + 2;
		pos = str.indexOf(DLM, pos+1);
	}
	
	//** Push any remaining characters onto the array
	outputArr.push(str.substring(startPos));
	
	//** Return array
	return outputArr;
}

function safeStr(str){
	var tmpStr = str.replace("|", "\\|");
	return tmpStr;
}

function elemExists(strIdName){
	//** length is 0 if element doesn't exist
	if ($(strIdName).length > 0){
		return true;
	} else {
		return false;
	}
}

//** Shamelessly borrowed from: css-tricks.com/snippets/javascript/get-url-variables/
function getQueryVariable(variable) {
   var query = window.location.search.substring(1);
   var vars = query.split("&");
   for (var i=0;i<vars.length;i++) {
		   var pair = vars[i].split("=");
		   if(pair[0] == variable){return pair[1];}
   }
   return("");
}



//***************************************************//
//*				HEADER EVENT METHODS				*//
//***************************************************//
function onHeaderLoad(){
	if ( elemExists("#gear") ){
		$('#gear').bind('mouseover', openSubMenu); 
		$('#gear').bind('mouseout', closeSubMenu); 
	}
	if (elemExists("#btnMain")){
		$("#btnMain").click(onMainClick);
	}
	if (elemExists("#btnProfile")){
		$("#btnProfile").click(onProfileClick);
	}
	if (elemExists("#btnAdmin")){
		$("#btnAdmin").click(onAdminClick);
	}
	if (elemExists("#btnForum")){
		$("#btnForum").click(onForumClick);
	}
	if (elemExists("#btnGames")){
		$("#btnGames").click(onGamesClick);
	}
	if (elemExists("#btnDeveloper") && UserLevel >= 100){
		$("#btnDeveloper").click(onDeveloperClick);
		$("#btnDeveloper").show();
	}
	if (elemExists("#btnLogout")){
		$("#btnLogout").click(onLogoutClick);
	}
	
	//** Load main page content by default
	if (elemExists("#btnMain")){
		//** No wait fadeOut so it's not seen
		$("#innerBody").fadeOut(0, function(){$("#innerBody").load("main_content.html", "", function(){
			onMainLoad();
		})});
	}
}

function openSubMenu() { 
	$(this).find('.myMenu').css('visibility', 'visible'); 
}

function closeSubMenu() { 
	$(this).find('ul').css('visibility', 'hidden');
}



//***************************************************//
//*				LOGIN EVENT METHODS					*//
//***************************************************//
function onLoginLoad(){
	if ( elemExists("#log1") ){
		$("#log1").click(onLoginClick);
	}
	if ( elemExists("#loginUser") ){
		$("#loginUser").focus();
		$("#loginUser").keyup(function(e) {
			if (e.keyCode == 13) {
				$( "#log1" ).click();
			}
		});
	}
	if ( elemExists("#loginPass") ){
		$("#loginPass").keyup(function(e) {
			if (e.keyCode == 13) {
				$( "#log1" ).click();
			}
		});
	}
	if ( elemExists("#loginError") ){
		$("#loginError").hide();
	}

	$( "#passInput" ).keyup(function(e) {
		if (e.keyCode == 13) {
			$( "#btnSendLogin" ).click();
		}
	});
}
function onLoginClick(){
	var uName = $("#loginUser").val(), 
		uPass = $("#loginPass").val(),
		strSend;
		
	if (uName == ""){
		onLoginError("Invalid username.");
		return;
	}
	if (uPass == ""){
		onLoginError("Invalid password.");
		return;
	}
	
	//** Compress string and send
	strSend = "" + uName + DLM + uPass;
	$.ajax({url: cgiUrl + "login.cgi", data: strSend, type: "POST", error: onLoginError, success: onLoginSuccess});
}
function onLoginSuccess(data){
	var dataArr = splitData(data);

	if (dataArr[0] == "ERROR"){
		onLoginError(dataArr);
		return;
	}
	sesKey = dataArr[1];
	UserID = parseInt(dataArr[2]);
	UserName = dataArr[3];
	UserLevel = parseInt(dataArr[4]);
	MuteLevel = parseInt(dataArr[5]);
	BanLevel = parseInt(dataArr[6]);
	
	//** Hide content, load new page, show content
	$("#contentBody").fadeOut(fadeOutSpeed, function(){$("#contentBody").load("main.html", "", function(){
		$("#contentBody").fadeIn(fadeInSpeed);
		onHeaderLoad();
	})});
}
function onLoginError(err){
	if ( $.type(err) == "array" ){
		$("#loginError").html(err[1]);
	} else if ( $.type(err) == "string" ){
		$("#loginError").html(err);
	} else if ( $.type(err) == "number" ){
		$("#loginError").html("Error Code: " + err);
	}
	$("#loginError").show();
}



//***************************************************//
//*				LOGIN EVENT METHODS					*//
//***************************************************//
function onLogoutClick(){
	$.ajax({url: cgiUrl + "logout.cgi", data: sesKey, type: "POST", error: onLogoutSuccess, success: onLogoutSuccess});
}
function onLogoutSuccess(data){
	sesKey = "";

	//** Hide content, load new page, show content
	document.location.href = "index.html";
}



//***************************************************//
//*				REGISTER EVENT METHODS				*//
//***************************************************//
function onRegisterLoad(){
	if ( elemExists("#reg1") ){
		$("#reg1").click(onRegisterClick);
	}
	if ( elemExists("#rUsername") ){
		$("#rUsername").focus();
	}
	if ( elemExists("#regError") ){
		$("#regError").hide();
	}
	
	//** Set hidden element value to the GET value r
	if (elemExists("#rRegcode")){
		var regVal = getQueryVariable("r");
		if (regVal == ""){
			//** Auto-redirect if value of r is non-existent
			document.location.href = "index.html";
			return;
		}
		$("#rRegcode").val(regVal);
	}
}
function onRegisterClick(){
	var strSend;
	
	if ($("#rUsername").val().length < 5){
		onRegisterError("Username must be at least 6 characters long.");
		return;
	}
	if ($("#rPassword").val().length < 6){
		onRegisterError("Password must be at least 6 characters long.");
		return;
	}
	if ($("#rPassword").val() != $("#rPassword2").val()){
		onRegisterError("Passwords do not match.");
		return;
	}
	
	//** Compress string and send
	strSend = $("#rUsername").val() + DLM + 
			  $("#rPassword").val() + DLM + 
			  $("#rFname").val() + DLM + 
			  $("#rLname").val() + DLM + 
			  $("#rEmail").val() + DLM +
			  $("#rRegcode").val();
	$.ajax({url: cgiUrl + "register.cgi", data: strSend, type: "POST", error: onRegisterError, success: onRegisterSuccess});
}
function onRegisterSuccess(data){
	var dataArr = splitData(data);

	if (dataArr[0] == "ERROR"){
		onRegisterError(dataArr);
		return;
	}
	
	//** Pass through to login success because it does the same thing on success
	onLoginSuccess(data);
}
function onRegisterError(err){
	if ( $.type(err) == "array" ){
		$("#regError").html(err[1]);
	} else if ( $.type(err) == "string" ){
		$("#regError").html(err);
	} else if ( $.type(err) == "number" ){
		$("#regError").html("Error Code: " + err);
	}
	$("#regError").show();
}

//***************************************************//
//*					ADMIN METHODS					*//
//***************************************************//

//***************************************************//
//*					PAGE METHODS					*//
//***************************************************//
function onMainLoad(){
	if (elemExists("#btnAddChat")){
		$("#btnAddChat").click(onChatButton);
		$("#txtAddChat").keypress(
		function(evt){
			if(evt.which == 13){
				$("#btnAddChat").click();
				return false;
			}
		});
		
	}
	if (elemExists("#topMenu_Wait")){
		$("#topMenu_Wait").hide();
	}
	if (elemExists("#topMenu_Start")){
		$("#topMenu_Start").hide();
	}
	
	requestMainContent();
}
function requestMainContent(){
	$.ajax("cgi-bin/games.cgi", {data: sesKey + DLM + GAMES_GAMES, type: "POST", success: function(data){
		onMainGameReceive(data);
		$("#innerBody").fadeIn(300);
	}, error: onMainGameReceive});
}
function onMainGameReceive(data){
	var gList = document.getElementById("gameList");
	var sList = document.getElementById("sessionList");
	var dCount = 5;
	
	data = splitData(data);
	
	//** Clear game list table
	while (gList.childNodes.length > 1){
		gList.removeChild(gList.lastChild);
	}
	
	//** Add games to page
	var gameCount = parseInt(data[1]);
	for (var i=0;i<gameCount;i++){
		addGameLine(data[2 + (i * dCount) + 1], 
					data[2 + (i * dCount) + 3], 
					data[2 + (i * dCount) + 2], 
					data[2 + (i * dCount)], 
					parseInt(data[2 + (i * dCount) + 4]), 
					"#gameList");
	}
	
	//** Clear session list table
	while (sList.childNodes.length > 1){
		sList.removeChild(sList.lastChild);
	}
	
	if (elemExists("#msgBox")){
		if (chatInstances == 0){
			chatInstances++;
			requestUpdate("START");
		}
	}
}
function onMainClick(){
	$("#innerBody").fadeOut(fadeOutSpeed, function(){$("#innerBody").load("main_content.html", "", function(){
		onMainLoad();
		$("#innerBody").fadeIn(fadeInSpeed);
	})});
}
function onGameClick(e){
	
}

function onForumClick(){
	$("#innerBody").fadeOut(fadeOutSpeed, function(){$("#innerBody").load("hubForum.html", "", function(){
		onForumLoad();
	})});
}

function onRegRequest(){
	var fName = $("#reqFName").val(),
		lName = $("#reqLName").val(),
		email = $("#reqEmail").val();
	
	//** TODO: Better error Checking
	if (fName == ""){
		$("#requestError").innerHTML = "Invalid first name.";
		return;
	}
	if (lName == ""){
		$("#requestError").innerHTML = "Invalid last name.";
		return;
	}
	if (email == ""){
		$("#requestError").innerHTML = "Invalid email.";
		return;
	}
	$("#requestError").innerHTML = "";
	
	var sendMSG = MAIN_REGREQUEST + DLM + email + DLM + fName + DLM + lName;
	$.ajax("cgi-bin/regrequest.cgi", {data: sendMSG, type: "POST", success: function(data){
		var dataArr = splitData(data);
		if (dataArr[0] == "ERROR"){
			$("#requestError").innerHTML = dataArr[1];
		} else {
			$("#loginError").innerHTML = "Registration request was sent.";
			toggleLogin();
		}
	}});
}

function onGamesLoad(){
}
function onGamesClick(){
	$("#innerBody").fadeOut(fadeOutSpeed, function(){$("#innerBody").load("games.html", "", function(){
		onGamesLoad();
		$("#innerBody").fadeIn(fadeInSpeed);
	})});
}
function onProfileClick(){
	$("#innerBody").fadeOut(fadeOutSpeed, function(){$("#innerBody").load("profile.html", "", function(){
		onProfileLoad();
	})});
}
function onDeveloperClick(){
	$("#innerBody").fadeOut(fadeOutSpeed, function(){
		$.ajax("cgi-bin/devconsole.cgi", {data: sesKey + DLM + "DISPLAY", type: "POST", success: function(data){
			$(innerBody).html(data);
			$("#innerBody").fadeIn(fadeInSpeed);
		}, error: function(data){
			$(innerBody).html(data);
			$("#innerBody").fadeIn(fadeInSpeed);
		}})
	});
}

//***************************************************//
//*			GLOBAL PAGE LOAD METHODS				*//
//***************************************************//
function loadPage(page, target){
	//** Changes the content of the entire page (including header) with the page at <page>
	//** Store variables globally for convenient event access
	nextURL = page;
	pageTarget = target;
	
	//** Fade out and switch content
	$(pageTarget).fadeOut(fadeOutSpeed, function(){$("#contentBody").load(nextURL, "", function(){
		$(pageTarget).fadeIn(fadeInSpeed, function(){
			onLoadPage();
		})
	})});
}
function onLoadPage(){
	if (elemExists("#btnAddChat")){
		$("#btnAddChat").click(onChatButton);
	}
	if (elemExists("#topMenu_JoinGame")){
		$("#topMenu_Wait").hide();
	}
	if (elemExists("#msgBox")){
		if (chatInstances == 0){
			chatInstances++;
			requestUpdate("START");
		}
	}
}



//***************************************************//
//*					CHAT METHODS					*//
//***************************************************//
function onChatUpdate(data){
	var dataArr = splitData(data),
		tmpName,
		tmpMsg,
		tmpColor;
	
	//** No data received so return nothing
	if (data == "" || dataArr.length < 3){
		return;
	}
	
	//** Add error message to chat
	if (dataArr[0] == "ERROR"){
		var newDiv = createChatLineDiv(dataArr[0], dataArr[1], "#FF6666");
		$("#msgBox").append(newDiv);
		var objDiv = document.getElementById("msgBox");
		objDiv.scrollTop = objDiv.scrollHeight;
		return;
	}
	
	//** Update time
	lastUpdate = parseInt(dataArr[0]);
	
	//** Add all new lines to chat
	for (var i=1;i<dataArr.length;i+=3){
		tmpName = dataArr[i];
		tmpMsg = dataArr[i+1];
		tmpColor = dataArr[i+2];
		
		var newDiv = createChatLineDiv(tmpName, tmpMsg, "#" + tmpColor);
		$("#msgBox").append(newDiv);
	}
	
	//** Force scroll test
	var objDiv = document.getElementById("msgBox");
	objDiv.scrollTop = objDiv.scrollHeight;
}
function onChatButton(){
	var chatTxt = $("#txtAddChat").val(),
		trimTxt = chatTxt.trim();
	
	if (chatTxt == ""){ return; }
	if (chatTxt.length < 3){ return; }

	$("#txtAddChat").val("");
	
	if(chatTxt == "/pause"){
		if (pauseChat){
			var newDiv = createChatLineDiv("CMD", "chat unpaused", "#BBBBBB");
			$("#msgBox").append(newDiv);
			var objDiv = document.getElementById("msgBox");
			objDiv.scrollTop = objDiv.scrollHeight;
		} else {
			var newDiv = createChatLineDiv("CMD", "chat paused", "#BBBBBB");
			$("#msgBox").append(newDiv);
			var objDiv = document.getElementById("msgBox");
			objDiv.scrollTop = objDiv.scrollHeight;
		}
		pauseChat = !pauseChat;
		return;
	}
	
	if (trimTxt.length < 3){ return; }
	
	sendChatMessage(chatTxt);
}
function createChatLineDiv(nameStr, textStr, color){
	//** Error handling
	if (nameStr == ""){
		nameStr = "Anonymous";
	}
	if (typeof color2 == 'undefined'){
		color2 = color;
	}
	if (textStr[0] == '>') color = '#789922';
	
	var lineDiv = document.createElement('div');
	lineDiv.className = 'chatLine';
	
	var nameSpan = document.createElement('span');
	nameSpan.style.color = color2;
	$(nameSpan).text(nameStr + ":");
	
	var textSpan = document.createElement('span');
	textSpan.className = 'userText';
	textSpan.style.color = color;
	textSpan.style.marginLeft = "4px";
	$(textSpan).text(textStr);
	
	lineDiv.appendChild(nameSpan);
	lineDiv.appendChild(textSpan);
	return lineDiv;
}
function requestUpdate(msgCommand){
	//** Update only if necessary
	timeObj = new Date();
	if (typeof msgCommand == "undefined"){
		msgCommand = "UPDATE";
	}
	if (!pauseChat){
		if (timeObj.getTime() - updateWait > lastUpdate){
			jQuery.ajax( cgiUrl + "chatupdate.cgi", {"data": sesKey + DLM + msgCommand, "type": "POST"} ).done(onChatUpdate);
			if (elemExists("#msgBox")){
				setTimeout(requestUpdate, updateWait);
				return;
			}
		} else if (elemExists("#msgBox")){
			setTimeout(requestUpdate, lastUpdate - timeObj.getTime());
			return;
		}
	}
	chatInstances--;
}
function sendChatMessage(msg){
	//** Update only if necessary
	jQuery.ajax( cgiUrl + "chatsend.cgi", {"data": sesKey + DLM + msg, "type": "POST"} ).done(onChatUpdate);
}

//***************************************************//
//*				READY GAME METHODS					*//
//***************************************************//
function requestReadyUpdate(msgCommand){
	//** Update only if necessary
	if (getElem("topMenu_Wait").style.display == "none")
		return;
	
	timeObj = new Date();
	if (typeof msgCommand == "undefined"){
		msgCommand = PLAY_UPDATE;
	}
	if (timeObj.getTime() - readyWait > lastReadyUpdate){
		jQuery.ajax( cgiUrl + "play.cgi", {"data": sesKey + DLM + msgCommand, "type": "POST"} ).done(onGameUpdate);
		if (elemExists("#msgBox")){
			setTimeout(requestReadyUpdate, readyWait);
			return;
		}
	} else if (elemExists("#msgBox")){
		setTimeout(requestReadyUpdate, lastReadyUpdate - timeObj.getTime());
		return;
	}
	readyInstances--;
}
function onGameUpdate(data){
	var dataArr = splitData(data);
	if (dataArr[0] == PLAY_START){
		//** Initiate start game
		$("#wait_Status").text("Session full. Press start to enter the game.");
		$("#wait_Status").css("color", "#C0FF00");
	} else if (dataArr[0] == PLAY_UPDATE){
		//** Update player list
		$("#wait_Status").text("Waiting for players...");
		$("#wait_Status").css("color", "#C0C0FF");
	}
}


//***************************************************//
//*					GAME METHODS					*//
//***************************************************//
function addGameLine(name, desc, gameURL, gameID, playerCount, destObj){
	var newTR = '<div class="gameTR" onclick="onGameClick" data-game-Id="' + gameID + '">';

	//** Display the image (left box)
	newTR += '<div class="td gameTR_Left">';
		newTR += '<img width="200" height="125" src="' + gameURL + 'icon.png" onError="onImageError(event);">';
	newTR += '</div>';
	
	//** Display the right box
	newTR += '<div class="td gameTR_Right">';
	
		//** Display the name
		newTR += '<div class="gameInfoBox" style="text-align:center;text-decoration:underline;">' + name + '</div>';
		
		//** Display the desc
		newTR += '<div class="gameInfoBox gameDescBox">' + desc + '</div>';
	newTR += '</div>';
	
	//** Add the buttons bar
	newTR += '<div class="gameInfoBox" style="position:absolute;bottom:0px;right:0px;text-align:right;">';
	if (playerCount > 1){
		//** Multiplayer buttons
		if (UserLevel > 1){
			newTR += '<button type="button" onclick="showStartSession(event);"';
			newTR += ' data-game-id="' + gameID + '"';
			newTR += ' data-game-name="' + name + '"';
			newTR += ' data-game-players="' + playerCount + '"';
			newTR += ' data-game-url="' + gameURL + '"';
			newTR += '>Post Session</button>';
		}
		newTR += '<button type="button" onclick="updateSessionsList(event);">View Sessions</button>';
	} else {
		//** Single player buttons
		newTR += '<button type="button" onclick="startSinglePlayer(event);"';
		newTR += ' data-game-id="' + gameID + '"';
		newTR += ' data-game-url="' + gameURL + '"';
		newTR += '>Play Game</button>';
	}
	newTR += '</div>';
	
	//** End row
	newTR += '</div>';
	
	if (typeof destObj == "string"){
		$(destObj).append(newTR);
	} else {
		destObj.append(newTR);
	}
}
function addSessionLine(name, players, maxPlayers, destObj){
	var tmpName = name;
	
	//** Add ellipsis
	if (tmpName.length > 12){
		tmpName = tmpName.substring(0,12) + "...";
	}
	var newTR = '<div class="sessionLine">';
		newTR += '<div class="sessionName">' + tmpName + '</div>';
		newTR += '<div class="sessionPlayers">' + players + '/' + maxPlayers + '</div>';
		newTR += '<div class="sessionJoin"><a href="#" style="margin-right:5px;">Join</a></div>';
	newTR += '</div>';
	if (typeof destObj == "string"){
		$(destObj).append(newTR);
	} else {
		destObj.append(newTR);
	}
}
function onGameReady(){
	var gDLM = "&";
	var sendStr = "";
}
function onGameQuit(){
	$("#topMenu_Wait").fadeOut(300, function(){$("#topMenu_JoinGame").fadeIn(300)});
}
function onStartCancel(){
	$("#joinGame_Cover").fadeOut(300);
}
function showStartSession(event){
	var gameID = event.target.dataset.gameId,
		gameName = event.target.dataset.gameName,
		gamePlayers = event.target.dataset.gamePlayers,
		gameMaxPlayers = gamePlayers,
		gameURL = event.target.dataset.gameUrl;
	
	$("#start_GameID").val(gameID);	
	$("#start_GameURL").val(gameURL);	
	$("#start_GameName").html(gameName);
	$("#start_SessionName").val("(" + gameName + ")");
	$("#start_Password").val("");
	$("#start_totalPlayers").html(gamePlayers);
	$("#start_maxPlayers").html(gameMaxPlayers);
	$("#start_PlayerRange").attr("min", gamePlayers).attr("max", gameMaxPlayers).val(gamePlayers); 
	$("#start_maxPlayers").html(gamePlayers);
	
	$("#joinGame_Cover").fadeIn(300);
}
function waitForGame(event){
	$("#topMenu_JoinGame").fadeOut(300, function(){
		var gameID = $("#start_GameID").val(),
			gamePass = $("#start_Password").val();
			sessionName = $("#start_SessionName").val(),
			maxPlayers = $("#start_PlayerRange").val();
			
		//** Start update cycle
		jQuery.ajax( cgiUrl + "play.cgi", {"data": getCmdMsg(GAMES_POST, [gameID, sessionName, maxPlayers, gamePass]), "type": "POST"} ).done(function(data){
			onGameSessionStart(data);
		});
	});
}
function onGameSessionStart(data){
	//** Handle initial data
	var dataArr = splitData(data);
	
	var gameID = $("#start_GameID").val(),
		gameName = $("#start_GameName").html(),
		gamePass = $("#start_Password").val();
		sessionName = $("#start_SessionName").val(),
		gamePlayers = [],//event.target.dataset.gamePlayers,
		maxPlayers = $("#start_PlayerRange").val(),
		//gameURL = event.target.dataset.gameUrl;
		
	$("#joinGame_Cover").fadeOut(0);
	
	$("#wait_GameName").html(gameName + " Lobby");
	$("#wait_SessionName").html($("#start_SessionName").val());
	if ($("#start_Password").val() != ""){
		$("#wait_SessionPass").html("(private)");
	} else {
		$("#wait_SessionPass").html("(none)");
	}
	$("#totalPlayers").html(1);
	$("#maxPlayers").html(maxPlayers);
	
	//** Clear player list
	var playerListBox = getElem("playerListBox");
	while (playerListBox.hasChildNodes()){
		playerListBox.removeChild(playerListBox.firstChild);
	}
	for (var i=0;i<gamePlayers;i++){
		var newPlayerLine = document.createElement("div");
		newPlayerLine.innerHTML = "" + i + ": ";
		playerListBox.addChild(newPlayerLine);
	}
	
	$("#wait_Status").text("Waiting for players...");
	$("#wait_Status").css("color", "#C0C0FF");
	$("#topMenu_Wait").fadeIn(300);
	
	//** Begin update loop
	if (readyInstances == 0){
		readyInstances++;
		requestReadyUpdate();
	}
}
function onGameSession(data){
	
}
function startSinglePlayer(event){
	var gameURL = event.target.dataset.gameUrl;
	window.open(gameURL + "?sp001&1&player1&player1&" + sesKey, '_blank');
}
function openGame(gameURL){
	var gameWin = window.open(gameURL, '_blank');
	gameWin.focus();
}
function updateSessionsList(e){
	jQuery.ajax( cgiUrl + "main.cgi", {"data": sesKey + DLM + MAIN_SESSIONLIST + DLM + e.target.dataset.gameId, "type": "POST"} ).done(onSessionUpdate);
}
function onSessionUpdate(data){
	var sesList = document.getElementById("sessionList"),
		sesCount = 0;
	while (sesList.hasChildNodes()){
		sesList.removeChild(sesList.lastChild);
	}
	
	data = splitData(data);
	if (data[0] == "ERROR") return;
	
	sesCount = parseInt(data[1]);
	for (var i=0;i<sesCount;i++){
		addSessionLine(data[2 + (i*3)], data[2 + (i*3)], data[3 + (i*3)], "#sessionList");
	}
}
function onImageError(event){
	console.log("IMAGE ERROR: loaded default image");
	event.target.src = 'img/hub5.png';
}
function getCmdMsg(cmd, arr){
	var strRet = sesKey + DLM + cmd;
	if (typeof arr != "undefined"){
		for (var i=0;i<arr.length;i++){
			strRet += DLM + arr[i];
		}
	}
	return strRet;
}
function toggleLogin(){
	$("#requestLogin").toggle();
	$("#loginBox").toggle();
}
function toggleRegister(){
	$("#loginBox").toggle();
	$("#enterRegCode").toggle();
}
function regCodeRedirect(){
	var regCode = $("#regCodeInput").val(),
		newDir = "reg.html?r=" + regCode;
	if (regCode == ""){
		$("#regCodeError").innerHTML = "Invalid registration code.";
		return;
	}
	document.location.href = newDir;
}


//****** ONLOAD ******//
$(document).ready(function(){
	//** Run every possible load method just in case.
	onLoginLoad();
	onRegisterLoad();
});