var apiUrl;

// functions
var getClosestStop
, getCurrentPosition
, sendRequest
, sendMessage;

/* Init shit
--------------------------------------------------------------- */
// apiUrl = 'http://api.visuweb.no/bybussen/5.0/Departure/Route/16011333/7c19bd89eb3476d2e17fd11fd816ff65';
apiUrl = 'http://api.visuweb.no/bybussen/6.0/Departure/Nearby/';

Pebble.addEventListener("ready",
  function (e) {
    getCurrentPosition();
  }
);

Pebble.addEventListener("appmessage",
  function(e) {
    switch (e.payload["10"]) {
      case "refresh":
      getCurrentPosition();
        console.log("Refreshing");
        break;
    }
  }
);

/* Functions and shit
--------------------------------------------------------------- */
getClosestStop = function () {

};

getCurrentPosition = function () {
  var options
  , success
  , error;

  options = {
    enableHighAccuracy: true,
    timeout: 5000,
    maximumAge: 0
  };

  success = function (position) {
    var url = apiUrl + position.coords.latitude + '/' + position.coords.longitude;
    // var url = apiUrl + '63.4184/10.404';
    sendRequest(url);
    console.log("Position: " + position.coords.latitude + "-" + position.coords.longitude);
  };

  error = function (error) {
    console.log("Couldn't get position");
  };

  navigator.geolocation.getCurrentPosition(success, error, options);
};

sendRequest = function (url) {
  var req = new XMLHttpRequest();

  req.open('GET', url, true);

  req.onload = function(e) {
    if (req.readyState == 4 && req.status == 200) {
      if (req.status == 200) {
        var data = JSON.parse(req.responseText);

        var dict = {};

        if (data.nearest_stop != null && data.nearest_stop.name != null) {
          dict[0] = data.nearest_stop.name.substring(0,16);
        }

        if (data.nearest_stop != null && data.nearest_stop.retning != null) {
          dict[1] = data.nearest_stop.retning;
        }

        if (data.next[0] != null) {
          dict[2] = data.next[0].l;
          dict[3] = data.next[0].t;
        }

        if (data.next[1] != null) {
          dict[4] = data.next[1].l;
          dict[5] = data.next[1].t;
        }

        if (data.next[2] != null) {
          dict[6] = data.next[2].l;
          dict[7] = data.next[2].t;
        }

        if (data.next[3] != null) {
          dict[8] = data.next[3].l;
          dict[9] = data.next[3].t;
        }

        sendMessage(dict);

      }
      else {
        console.log("Error");
      }
    }
  }

  req.send(null);
};

sendMessage = function (data) {
  Pebble.sendAppMessage(data, 
    function (e) {
      console.log("Message sent");
    },
    function (e) {
      console.log("Unable to deliver message with transactionId="
      + e.data.transactionId
      + " Error is: " + JSON.stringify(e)
      + " Data: " + JSON.stringify(data));
    }
  );
};


