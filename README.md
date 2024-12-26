# flood-alert
Arduino Nano IoT Flood Alert device

Flood Alert
![Flood Alert Circuit Diagram](fritzing/flood_alert_bb.jpg).

## Introduction
The UK Environment Agency flood-monitoring API provides developers with access to near real time information covering:

- Flood warnings and flood alerts
- Flood areas which to which warnings or alerts apply
- Measurements of water levels and flows
- Information on the monitoring stations providing those measurements

Water levels and flows are regularly monitored, usually every 15 minutes. However, data is transferred back to the Environment Agency at various frequencies, usually depending on the site and level of flood risk. Transfer of data is typically once or twice per day, but usually increases during times of heightened flood risk.

These APIs are provided as open data under the Open Government Licence with no requirement for registration. If you make use of this data please acknowledge this with the following attribution statement:

"this uses Environment Agency flood and river level data from the real-time data API (Beta)"

## Getting your flood warning area
This is the easiest way to find out your flood area but there needs to be an alert or warning at the time:

You can get all the flood warnings in the UK in you browser by visiting the following URL:
[Flood alerts and warnings](https://check-for-flooding.service.gov.uk/alerts-and-warnings)

Click on the warning you are interested in for example River Windrush form Bourton to Newbridge:
[https://check-for-flooding.service.gov.uk/target-area/061WAF10Windrush](https://check-for-flooding.service.gov.uk/target-area/061WAF10Windrush)

The last part of the URL above is the flood warning area code in this example it is 061WAF10Windrush

If there is no flood warning for your area you can find the url from your Lat / Long coordinates.
For example Witney in Oxfordshire has lat: 51.7859° N, long: 1.4851° W
North is positive, West is negative.

Put these into the following URL 

http://environment.data.gov.uk/flood-monitoring/id/floodAreas?lat=y&long=x&dist=d

Where: y is the lat, x is the long and d is the distance in km from your location. Start with close distances and use larger values if you don't get any results.

So for Witney the URL is (Note the negative W coordinate):
https://environment.data.gov.uk/flood-monitoring/id/floodAreas?lat=51.7859&long=-1.4851&dist=0.5

Paste this into a browser and you will get a list of possible Flood Areas. Here's the result for Witney:
```
{ 
  "@context" : "http://environment.data.gov.uk/flood-monitoring/meta/context.jsonld" ,
  "meta" : { 
    "publisher" : "Environment Agency" ,
    "licence" : "http://www.nationalarchives.gov.uk/doc/open-government-licence/version/3/" ,
    "documentation" : "http://environment.data.gov.uk/flood-monitoring/doc/reference" ,
    "version" : "0.9" ,
    "comment" : "Status: Beta service" ,
    "hasFormat" : [ "http://environment.data.gov.uk/flood-monitoring/id/floodAreas.csv?lat=51.7859&long=-1.4851&dist=0.5", "http://environment.data.gov.uk/flood-monitoring/id/floodAreas.rdf?lat=51.7859&long=-1.4851&dist=0.5", "http://environment.data.gov.uk/flood-monitoring/id/floodAreas.ttl?lat=51.7859&long=-1.4851&dist=0.5", "http://environment.data.gov.uk/flood-monitoring/id/floodAreas.html?lat=51.7859&long=-1.4851&dist=0.5" ] ,
    "limit" : 500
  }
   ,
  "items" : [ { 
    "@id" : "http://environment.data.gov.uk/flood-monitoring/id/floodAreas/061FWF10Witney" ,
    "county" : "Oxfordshire" ,
    "description" : "River Windrush at Witney and Ducklington down to and including Hardwick" ,
    "eaAreaName" : "Thames" ,
    "floodWatchArea" : "http://environment.data.gov.uk/flood-monitoring/id/floodAreas/061WAF10Windrush" ,
    "fwdCode" : "061FWF10Witney" ,
    "label" : "River Windrush at Witney and Ducklington" ,
    "lat" : 51.75826 ,
    "long" : -1.45441 ,
    "notation" : "061FWF10Witney" ,
    "polygon" : "http://environment.data.gov.uk/flood-monitoring/id/floodAreas/061FWF10Witney/polygon" ,
    "quickDialNumber" : "171073" ,
    "riverOrSea" : "River Windrush"
  }
  , { 
    "@id" : "http://environment.data.gov.uk/flood-monitoring/id/floodAreas/061WAF10Windrush" ,
    "county" : "Gloucestershire, Oxfordshire" ,
    "description" : "River Windrush from Bourton to Newbridge including Burford, Asthall, Minster Lovell, Crawley, Witney, Ducklington, Rack End and Standlake" ,
    "eaAreaName" : "Thames" ,
    "fwdCode" : "061WAF10Windrush" ,
    "label" : "River Windrush from Bourton to Newbridge" ,
    "lat" : 51.8284 ,
    "long" : -1.65082 ,
    "notation" : "061WAF10Windrush" ,
    "polygon" : "http://environment.data.gov.uk/flood-monitoring/id/floodAreas/061WAF10Windrush/polygon" ,
    "quickDialNumber" : "171075" ,
    "riverOrSea" : "River Windrush"
  }
...
```
Use the "description" key to find the nearest area to you and then take the "@id" url for that area. For example:
"@id" : "http://environment.data.gov.uk/flood-monitoring/id/floodAreas/061WAF10Windrush"

Now take the last part of the "@id" url which is the key for that area, in this case "061WAF10Windrush"

Copy and paste the key into the config.h file:
```
// Your flood area code from Gov.co.uk Flood Warning Service
#define AREA_CODE "061WAF10Windrush"
```

## Testing
You can test the API for your flood area by making a request in your browser to the following URI:
http://environment.data.gov.uk/flood-monitoring/id/floodAreas/{your-flood-area-code}

So in the example above for the Witney use this URI:
[http://environment.data.gov.uk/flood-monitoring/id/floodAreas/061WAF10Windrush](http://environment.data.gov.uk/flood-monitoring/id/floodAreas/061WAF10Windrush)

This will return a JSON object with all the flood information for that area.

## API Documentation:
[Flood API](http://environment.data.gov.uk/flood-monitoring/doc/reference#flood-warnings)

## Flood Alert display
The device polls the API at the frequency you set and it receives the API response and displays the following data:

An icon representing the flood warning which may be at one of four possible severity levels - derived from the severityLevel:

Level	Name				    Meaning
1. Severe Flood Warning		    Severe Flooding, Danger to Life.
2. Flood Warning			    Flooding is Expected, Immediate Action Required.
3. Flood Alert                  Flooding is Possible, Be Prepared.
4. Warning no Longer in Force   The warning is no longer in force

The severity level description - derived from severity

The update date / time - derived from timeRaised (The date and time the warning was last reviewed. Usually (but not always) this leads to a change in the message or severity)

## Configuration
User config is in "config.h" Add your WiFi details and Flood Area there. You can also set 3 lines of upto 17 characters to describe the flood area.

## Demo Mode
Hold down the Demo button and press the Reset button to enter Demo Mode. Press Reset again to exit back to Standard Mode.

## Building
To build the code in the Arduino IDE the following libraries must be installed:
  [Arduino WiFiNINA](https://github.com/arduino-libraries/WiFiNINA)
  [Benoît Blanchon ArduinoJson](https://arduinojson.org/)
  [Evert-arias EasyButton](https://github.com/evert-arias/EasyButton)

The Waveshare files must be obtained from GitHub at the following [URL](https://github.com/waveshareteam/e-Paper) 
Copy the following files from the Arduino/epd2in9_V2 directory into the FloodAlert directory:
```
epd2in9_V2.cpp
epd2in9_V2.h
epdif.cpp
epdif.h
epdpaint.cpp
epdpaint.h
font8.cpp
font12.cpp
font.16.cpp
font.20.cpp
font.24.cpp
fonts.h
```
Edit the pin definitions in epdif.h so they are like this:
```
// Pin definition
#define RST_PIN         14
#define DC_PIN          7
#define CS_PIN          8
#define BUSY_PIN        4
#define PWR_PIN         7
```
Set your WiFi credentials and flood area code in config.h then compile and upload the code to your Nano IoT. 

## License
The code is released under the MIT License


