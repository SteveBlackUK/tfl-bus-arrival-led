var request = require("request");

// The Cities IDs can be found on openweathermap.org (make a search, and look the URI)
var cities = [2988507, 5391959];
var slackBotUri = ""; // TODO: Complete

request("http://api.openweathermap.org/data/2.5/group?id="+cities.join(',')+"&units=metric ", function(error, response, body) {
  // Maybe we can handle this differently/better ?
  if(error != null)
  	return;
  	
  var text = "Hello team, here is the weather forecast for today: \n";

  var weatherForecasts = JSON.parse(body);
 
  for (var i = weatherForecasts.list.length - 1; i >= 0; i--) {
  	var currentCity= weatherForecasts.list[i];

  	text += "*"+currentCity.name+"*: ";
  	text += ":" + currentCity.weather[0].icon + ": ";
  	text += currentCity.weather[0].main + ", " + currentCity.weather[0].description + ". ";
  	text += "Temp: " + currentCity.main.temp + "°c. "
  	text += "\n";
  };

  console.log(text);

  request.post({
        url: slackBotUri,
         body: text
         }, function(error, response, body){
            console.log(body);
    });
});