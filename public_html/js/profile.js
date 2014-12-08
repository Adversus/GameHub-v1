//** Editted by Zach Shroeder

function profileHideAll(){
	$("#homediv").hide();
	$("#achievementsdiv").hide();
	$("#inboxdiv").hide();
	$("#searchdiv").hide();
	$("#managediv").hide();
	$("#settingsdiv").hide();
	$("#uploaddiv").hide();
	$("#editdiv").hide();
}
function setProfileButtons(){
	$("#home").click(function(){
		profileHideAll();
		$("#homediv").show();
	});
	$("#achievements").click(function(){
		profileHideAll();
		$("#achievementsdiv").show();
	});
	$("#inbox").click(function(){
		profileHideAll();
		$("#inboxdiv").show();
	});
	$("#search").click(function(){
		profileHideAll();
		$("#searchdiv").show();
	});
	$("#manage").click(function(){
		profileHideAll();
		$("#managediv").show();
	});
	$("#settings").click(function(){
		profileHideAll();
		$("#settingsdiv").show();
	});
	$("#upload").click(function(){
		profileHideAll();
		$("#uploaddiv").show();
	});
	$("#edit").click(function(){
		profileHideAll();
		$("#editdiv").show();
	});
}
function onProfileLoad(data){
	//** setProfileButtons is located in profile.js
	setProfileButtons();
	
	$("#profileName").text(UserName);
	$("#profileLevel").text("-" + uLevelNames[UserLevel] + "-");
	
	//** Display ban labels
	var banStatus = "";
	if (MuteLevel > 0){
		banStatus += "<div style='color:FF6666;'>&lt;Muted&gt;</div>";
	}
	if (BanLevel > 0){
		banStatus += "<div style='color:FF6666;'>&lt;Banned&gt;</div>";
	}
	if (banStatus == ""){
		banStatus = "</br>";
	}
	$("#profileBanned").html(banStatus);
	
	//** Show profile
	$.ajax("cgi-bin/profile.cgi", {data: sesKey + DLM + "SHOWME", type: "POST", success: function(data){
		onProfileReceive(data);
		$("#innerBody").fadeIn(fadeInSpeed);
	}, error: function(data){
		data = splitData(data);
		$("#innerBody").fadeIn(fadeInSpeed);
	}});
}
function onProfileReceive(data){
	data = splitData(data);
	$("#profileName").text(data[1]);
	$("#profileLevel").text(data[2]);
	$("#inviteCode").val(data[5]);
	$("#profileFirstName").text(data[6]);
	$("#profileLastName").text(data[7]);
}
function sendNewRegRequest(){
	$.ajax("cgi-bin/profile.cgi", {data: sesKey + DLM + "NEWREG", type: "POST", success: function(data){
		data = splitData(data);
		$("#inviteCode").val(data[1]);
		$("#innerBody").fadeIn(fadeInSpeed);
	}, error: function(data){
		data = splitData(data);
		$("#innerBody").fadeIn(fadeInSpeed);
	}});
}