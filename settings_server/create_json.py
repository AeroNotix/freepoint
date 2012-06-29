import simplejson
import MySQLdb

json_data = {
    "user_id": {
        "TYPE": "VARCHAR",
        "LENGTH": 105,
        "VALIDATORS": ["UNIQUE"]
        },
    "user_type": {
        "TYPE": "CHOICE",
        "CHOICES": ["RUSER", "ADMIN", "TEAML"]
        },
    "uMarket": {
        "TYPE": "CHOICE",
        "CHOICES": ["BK", "CZ", "BG"]
        },
    "uProcess": {
        "TYPE": "CHOICE",
        "CHOICES": ["AP", "AR", "AO"]
        },
    "breakLength": {
        "TYPE": "TIME"
        },
    "shiftLength": {
        "TYPE": "TIME"
        },
    "Start_Date": {
        "TYPE": "DATE"
        }
    }

jsondump = simplejson.dumps(json_data)

conn = MySQLdb.connect(user="root",
                       passwd="root",
                       db="db_freepoint")
cur = conn.cursor()
cur.execute("""
INSERT INTO metadata
VALUES(NULL, 'db_timetracker.tbluser', %s)
""", jsondump)
conn.commit()
