import urllib2
import urllib
import simplejson

json_data = {
    "User":"aero",
    "Password": "something"
}

json_data2 = {
    "User":"123123",
    "Password": "sothing"
}

json_data3 = {
    "User":"",
    "Password": "sothing"
}

json_data4 = {
    "User":"123",
    "Password": ""
}

json_data5 = {}



for x in range(10):
    req = urllib2.Request("http://localhost:12345/login/", urllib.urlencode(json_data))
    req2 = urllib2.Request("http://localhost:12345/login/", urllib.urlencode(json_data2))
    req3 = urllib2.Request("http://localhost:12345/login/", urllib.urlencode(json_data3))
    req4 = urllib2.Request("http://localhost:12345/login/", urllib.urlencode(json_data4))
    req5 = urllib2.Request("http://localhost:12345/login/", urllib.urlencode(json_data5))

    print urllib2.urlopen(req).read()
    print urllib2.urlopen(req2).read()
    print urllib2.urlopen(req3).read()
    print urllib2.urlopen(req4).read()
    print urllib2.urlopen(req5).read()
    
