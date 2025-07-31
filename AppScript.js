function doPost(e) {
  try {
    var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
    var data = JSON.parse(e.postData.contents);
    var time = new Date();

    var nextRow = sheet.getLastRow() + 1; 

    
    var nextNum = nextRow - 1;

    var status = "Normal";
    if (data.temperature > 45) {
      status = "Danger: High Temp";
    } else if (data.co > 3500) {
      status = "Danger: CO";
    } else if (data.gas > 4000) {
      status = "Danger: Gas/Smoke";
    }

    sheet.appendRow([
      nextNum,
      time,
      data.temperature,
      data.humidity,
      data.co,
      data.gas,
      "disabled",
      status,
      "",
      data.gps || "N/A"
    ]);

    
    var range = sheet.getRange(2, 1, sheet.getLastRow() - 1, sheet.getLastColumn());
    var backgrounds = [];

    for (var i = 0; i < range.getNumRows(); i++) {
      if (i % 2 == 0) {
        // Bleu accentuation 1 clair à 70% (#D9E5F3 environ)
        backgrounds.push(["#D9E5F3".repeat(range.getNumColumns()).split("")]); 
      } else {
        backgrounds.push(new Array(range.getNumColumns()).fill(null));
      }
    }

    
    backgrounds = [];
    for (var i = 0; i < range.getNumRows(); i++) {
      var rowColors = [];
      for (var j = 0; j < range.getNumColumns(); j++) {
        rowColors.push(i % 2 == 0 ? "#D9E5F3" : null);
      }
      backgrounds.push(rowColors);
    }

    range.setBackgrounds(backgrounds);

    return ContentService.createTextOutput("Success").setMimeType(ContentService.MimeType.TEXT);

  } catch (err) {
    return ContentService.createTextOutput("Error: " + err.message).setMimeType(ContentService.MimeType.TEXT);
  }
}
