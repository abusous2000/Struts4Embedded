document.addEventListener('DOMContentLoaded', function(){ 
   refreshPlayerStatus();
}, false);
function execGETAjax(url, callback){
   var xhttp = new XMLHttpRequest();
   xhttp.onreadystatechange = function(){;
     if(this.readyState == 4 && this.status == 200){
       if ( callback != null ) 
          callback(xhttp);
     };
   };
   xhttp.open("GET", url, true);
   xhttp.send();
};
function refreshPlayerStatus(){
  execGETAjax("/getPlayerStatus",updatePlayerStatus);
}
function reloadMainPage(){
  execGETAjax("/updateMainPage");
}
function reloadAppInfo(){
  execGETAjax("/getAppInfo",getAppInfo);
}
function execPOSTAjax(url, payloadAsString, callback){
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function(){;
     if(this.readyState == 4 && this.status == 200){
       if ( callback != null ) 
           callback(xhttp);
     };
  };
  xhttp.open("POST", url, true);
  xhttp.send(payloadAsString);
};
function updatePlayerStatus(xhttp){
  if ( xhttp.responseText != null ){
    var playerStatus = JSON.parse(xhttp.responseText)
    document.getElementById("playerStatusID").innerHTML = playerStatus.status;
    document.getElementById("playerTimeRemainingID").innerHTML = playerStatus.timeRemaining;
    document.getElementById("playerVolID").innerHTML = playerStatus.vol;
    document.getElementById("trackID").innerHTML = playerStatus.track;
  }
}
function getAppInfo(xhttp){
  if ( xhttp.responseText != null ){
    var appInfo = JSON.parse(xhttp.responseText)
    document.getElementById("appNameID").innerHTML = appInfo.appName;
  }
}
function setStatusMsg(xhttp){
    if ( this.responseText != null ){
      console.log(xhttp.responseText);
      var respJson = JSON.parse(xhttp.responseText);
      document.getElementById("msg").innerHTML = respJson.msg;
      console.log(respJson.msg);
  }
  else
      console.log("not response text");
}
function toggleMuteActionEvent(){
   var payload         = {actionEvent:"toggleMute",data: "on"};
  execPOSTAjax("/actionEvent",JSON.stringify(payload),setStatusMsg);
};
function nextTrackActionEvent(){
   var payload         = {actionEvent:"nextTrack",data: "on"};
  execPOSTAjax("/actionEvent",JSON.stringify(payload),setStatusMsg);
};
function togglePauseActionEvent(){
  var payload         = {actionEvent:"togglePause",data: "on"};
  execPOSTAjax("/actionEvent",JSON.stringify(payload),setStatusMsg);
};
function setVolumeActionEvent( ){
  var vol             = document.getElementById("volID").value;
  var payload         = {actionEvent:"setVolume",data: vol };
  execPOSTAjax("/actionEvent",JSON.stringify(payload),setStatusMsg);
};
function sendActionEvent( ){
  var eventName             = document.getElementById("actionEventNameID").value;
  var eventData             = document.getElementById("actionEventDataID").value;
  var payload         = {actionEvent:eventName,data: eventData };
  execPOSTAjax("/actionEvent",JSON.stringify(payload),setStatusMsg);
};
