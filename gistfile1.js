
// var slackBotUri = hook.env.slackweather_url; // I'm using hook.io which allows you to store env variables separately to avoid exposing
// var openWeatherKey = hook.env.openweatherkey;
// This is a working weather call - but isn't for posting to Slack - so need to combine with the bottom

//  
function forecastUrl(apiKey, location) {
  var apiUrl = "https://api.forecast.io/forecast";
  var excludeFields = "exclude=minutely,hourly";
  return apiUrl + "/" + apiKey + "/" + location + "?" + excludeFields;
}

function getForecast(url) {
  var got = require("got");
  return got(url)
  .then(function (response) {
    return JSON.parse(response.body);
  });
}

function forecastMapper(weather) {
  return {
    // Word weather summary for next 24 hours
    summary: weather.daily.data[0].summary,
    // Current temperature
    temperature: weather.currently.apparentTemperature
  };
}

function clothesForWeather(weather) {
  var s = weather.summary;
  if(s.match(/rain|drizzle/i)) {
    return "umbrella";
  } else if(s.match(/snow/i)) {
    return "shovel";
  }
  
  var t = weather.temperature;
  if(t < 33) {
    return "hat";
  } else if(t < 55) {
    return "jacket";
  } else if(t < 70) {
    return "sweater";
  } else {
    return "tshirt";
  }
}

function outputMessage(hook, clothes, message) {
  if(hook.params.coreid) {
    console.log(message);
    hook.res.end(clothes);
  } else {
    hook.res.end(message + "\n");
  }
}

module['exports'] = function whatToWear (hook) {
  var location = hook.params.location || hook.env.DEFAULT_LOCATION;
  var url = forecastUrl(hook.env.FORECAST_IO_API_KEY, location);
  
  getForecast(url)
  .then(function (weather) {
    var simpleWeather = forecastMapper(weather);
    var clothes = clothesForWeather(simpleWeather);
    var message = "Wear " + clothes + " for forecast " + JSON.stringify(simpleWeather);
    outputMessage(hook, clothes, message);
  })
  .catch(function (error) {
    console.error("Error getting forecast: " + error);
    hook.res.end("error");
  });
};


///////////
// This was a function that worked for a simple post to slack - but needs to be merged with the above weather call
module['exports'] = function getLondonWeather(hook) {

  // hook.io has a range of node modules available - see
  // https://hook.io/modules.
  // We use request (https://www.npmjs.com/package/request) for an easy way to
  // make the HTTP request.
  var request = require('request');

  // The parameters passed in via the slash command POST request.
  var params = hook.params;

  // Check that the hook has been triggered from our slash command by
  // matching the token against the one in our environment variables.
  
  // TODO - Fix
  if(params.token === hook.env.slackweather_token) {

    // Set up the options for the HTTP request.
    var options = {

      // Use the Webhook URL from the Slack Incoming Webhooks integration.
      //TODO change
      uri: hook.env.slackweather_url,

      method: 'POST',

      // Slack expects a JSON payload with a "text" property.
      json: {
        'text': '@' + params.user_name + ' asked for the weather ',
        
        // Request that Slack parse URLs, usernames, emoji, etc. in the 
        // incoming text.
        'parse': 'full'
      }
    };

    // Make the POST request to the Slack incoming webhook.
    request(options, function (error, response, body) {
      
      // Pass error back to client if request endpoint can't be reached.
      if (error) {
        hook.res.end(error.message);
      }

      // Finally, send the response. This will be displayed to the user after
      // they submit the slash command.
      hook.res.end('High five success! Go to #highfives to see it :smile:');
    });

  } else {

    // If the token didn't match, send a response anyway for debugging.
    hook.res.end('Incorrect token.');
  }
};
