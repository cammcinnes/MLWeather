//-----------------------------------------------
//Originally published by Mogsdad@Stackoverflow
//Modified for jarkomdityaz.appspot.com
//Modified for Hackster.io by Stephen Borsay
//Modified for Cameron McInnes
//-----------------------------------------------
/*

GET request query:

https://script.google.com/macros/s/<gscript id>/exec?celData=data_here
----------------------------------------------------------------------

GScript, PushingBox and Arduino/ESP8266 Variables in order:

temperatureData
pressureData
humidityData
gasResistance
airQualityScore
humidityScore
gasScore
altitudeData
----------------------------------------------------
*/

/* Using spreadsheet API */

function doGet(e) {
  Logger.log(JSON.stringify(e)); // view parameters

  var result = "Ok"; // assume success

  if (e.parameter == undefined) {
    result = "No Parameters";
  } else {
    var id = "myGoogleSheet"; //docs.google.com/spreadsheetURL/d
    var sheet = SpreadsheetApp.openById(id).getActiveSheet();
    var newRow = sheet.getLastRow() + 1;
    var rowData = [];
    //var waktu = new Date();
    rowData[0] = new Date(); // Timestamp in column A

    for (var param in e.parameter) {
      Logger.log("In for loop, param=" + param);
      var value = stripQuotes(e.parameter[param]);
      //Logger.log(param + ':' + e.parameter[param]);
      switch (param) {
        case "temperatureData": //Parameter
          rowData[1] = value; //Value in column B
          break;
        case "pressureData":
          rowData[2] = value;
          break;
        case "humidityData":
          rowData[3] = value;
          break;
        case "gasResistance":
          rowData[4] = value;
          break;
        case "airQualityScore":
          rowData[5] = value;
          break;
        case "humidityScore":
          rowData[6] = value;
          break;
        case "gasScore":
          rowData[7] = value;
          break;
        case "altitudeData":
          rowData[8] = value;
          break;
        default:
          result = "unsupported parameter";
      }
    }
    Logger.log(JSON.stringify(rowData));

    // Write new row below
    var newRange = sheet.getRange(newRow, 1, 1, rowData.length);
    newRange.setValues([rowData]);
  }

  // Return result of operation
  return ContentService.createTextOutput(result);
}

/**
 * Remove leading and trailing single or double quotes
 */
function stripQuotes(value) {
  return value.replace(/^["']|['"]$/g, "");
}
