CREATE TABLE IF NOT EXISTS parkingHistory (
    id INTEGER PRIMARY KEY,
    datetimeStamp TEXT,
    parkEvent TEXT,
    parkingSpotId INTEGER
)