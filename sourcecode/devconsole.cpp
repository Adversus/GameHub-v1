/***********************************************************
devconsole.cpp

A script testing console for independent development
************************************************************/
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <stdlib.h>
#include <ctype.h>
#include "libs/common.hpp"
#include "libs/query.hpp"
#include "libs/randStrHash.hpp"
#include "libs/sqltdb.hpp"

using namespace std;

const int pd_SKEY = 0,
		  pd_COMMAND = 1;

void sendDevPage();

int main(int argc, char* argv[])
{
	string postText = retrievePost(),
		   userID = "",
		   query = "",
		   tmpStr;
	int dbResult = 0,
		userLevel = 0;
	sqltWrap db;
	vector <string> userData;
	
	cout << "Content-Type: text/html\n\n";
	
	//** Split input into vector
	tokenizeStr(postText, DLM, userData);
	
	//** Error checking (force user to send skey and command)
	if (userData.size() < 1){
		cout << "ERROR" << DLM << "Invalid command" << endl;
		return 0;
	}
	
	//** Split input into vector
	tokenizeStr(postText, DLM, userData);
	if (userData.size() < 2){
		//** Default command
		userData[pd_COMMAND] = "DISPLAY";
	}
	
	//** Connect to DB
	db.open(DIR_DB);
	
	//** Confirm logged in
	dbResult = checkSession(db, userData[pd_SKEY].c_str());
	if (dbResult != DB_SUCCESS){
		cout << "ERROR" << DLM << "Failed to check session id [" << dbResult << "]" << endl;
		return 0;
	}
	if (db.numRows() == 0){
		cout << "ERROR" << DLM << "Not logged in.";
		return 0;
	}
	userID = db[0][0];
	
	//** Check user level
	query = "SELECT UserLevel FROM HubUsers WHERE UserID = ?";
	db.prepare(query);
	db.bind(1, userID);
	dbResult = db.runPrepared();
	if (dbResult != DB_SUCCESS){
		cout << "ERROR" << DLM << "Failed to check user level. [" << dbResult << "]" << endl;
		return 0;
	}
	
	//** Check user level
	tmpStr = db[0][0];
	userLevel = atol(tmpStr.c_str());
	if (userLevel < 100){
		cout << "ERROR" << DLM << "Invalid access level." << endl;
		return 0;
	}
	
	//** Handle commands
	if (userData[pd_COMMAND] == "DISPLAY" || userData[pd_COMMAND] == ""){
		sendDevPage();
		return 0;
	}
	cout << "Nothing happened. [" << userData[pd_COMMAND] << "]" << endl;
	return 0;
}

void sendDevPage(){
cout << "<div class='gameColumn'>" <<
		"<div class='contentMenu' style='background-color: #262626; margin: 10px; min-width:640px; padding: 15px; border-radius:20px; border: 2px solid #fdc163;margin-left:auto;margin-right:auto;'>" <<
		"<div class='inputBox'>" <<
			"<span class='inputLabel'>URL</span></br>" <<
			"<input id='inURL' type='text' style='width:300px;' value='main'>" <<
			"<input id='inFileType' type='text' style='width:50px;' value='.cgi'>" <<
		"</div>" <<
		"<div class='inputBox'>" <<
			"<span class='inputLabel'>Sending Data</span></br>" <<
			"<input id='inData' type='text' style='width:300px;'>" <<
			"<input id='btnAddToken' type='button' value='+DLM' />" <<
		"</div>" <<
		"<div class='inputBox'>" <<
			"<span class='inputLabel'>Sent...</span></br>" <<
			"<input id='sentData' type='text' style='width:500px;background-color:AAAAAA;'>" <<
		"</div>" <<
		"<div class='buttonBar'>" <<
			"<input id='btnSesKey' type='button' value='Session Key (enabled)' />&nbsp;" <<
			"<input id='btnSend' type='button' value='Send' />" <<
		"</div>" <<
	"</div>" <<
	"<div class='contentMenu' style='background-color: #262626; margin: 10px; padding: 15px; border-radius:20px; border: 2px solid #fdc163;margin-left:auto;margin-right:auto;'>" <<
		"<div class='inputBox'>" <<
			"<span class='inputLabel'>Recieved Data</span></br>" <<
			"<textarea id='outputTA' cols='60' rows='7'></textarea>" <<
		"</div>" <<
		"<div class='buttonBar'>" <<
		"</div>" <<
	"</div>" <<
	"<script type='text/javascript'>" <<
		//** Global Vars"
		"var receivedData = ''," <<
			"addSesKey = true;" <<
		
		"function showError(errorVal){" <<
			//** ToDo: Improve this
			"$('#outputTA').val(errorVal);" <<
		"}" <<
		
		//** Event Handlers
		"function onSendData(){" <<
			//** Send button event handler
			"var dataVal = $('#inData').val()," <<
				"destURL = 'cgi-bin/' + $('#inURL').val() + $('#inFileType').val();" <<
				
			"if (addSesKey){" <<
				"dataVal = sesKey + DLM + dataVal" <<
			"}" <<
			"$('#sentData').val(dataVal);" <<
				
			"if (destURL == ''){" <<
				"showError('ERROR: URL cannot be empty.');" <<
				"return;" <<
			"}" <<
			
			"$.ajax({data: dataVal, type:'POST', url: destURL, success: onReceiveData, error: onAjaxError});" <<
		"}" <<
		"function onReceiveData(data){" <<
			//** Data from server event handler
			"var datToken = $('#inToken').val();" <<
			"receivedData = data;" <<
			"$('#outputTA').val(data);" <<
		"}" <<
		"function onSesKeyToggle(){" <<
			"if (addSesKey){" <<
				"addSesKey = false;" <<
				"$('#btnSesKey').val('Session Key (disabled)')" <<
			"} else {" <<
				"addSesKey = true;" <<
				"$('#btnSesKey').val('Session Key (enabled)')" <<
			"}" <<
		"}" <<
		"function onDLMAdd(){" <<
			"$('#inData').val($('#inData').val() + DLM);" <<
			"$('#inData').focus();" <<
		"};" <<
		"function onAjaxError(jqXHR, textStatus, errorThrown){" <<
			//** ToDo: Improve this
			"showError(errorThrown);" <<
		"}" <<
		"function onDevEnter(evt){" <<
			"if (evt.which == 13){" <<
				"$('#btnSend').click();" <<
				"return false;" <<
			"}" <<
		"}" <<
		
		//** On document load
		"$(document).ready(function(){" <<
			//** Send button event set
			"$('#btnSend').click(onSendData);" <<
			"$('#btnSesKey').click(onSesKeyToggle);" <<
			"$('#btnAddToken').click(onDLMAdd);" <<
			"$('#inURL').keypress(onDevEnter);" <<
			"$('#inFileType').keypress(onDevEnter);" <<
			"$('#inData').keypress(onDevEnter);" <<
		"});" <<
	"</script></div>"	<< endl;	
}