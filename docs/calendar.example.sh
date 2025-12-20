#!/bin/bash
echo "Content-type: text/plain"
echo ""

# Set your Calendar IDs and API Key
calendarIds=("xxxxxxx@gmail.com" "xxxx@xxxxxx.com")
apiKey="xxxxxxxx"

# Get current time and end of today in ISO 8601 format
now=$(date -u +"%Y-%m-%dT%H:%M:%SZ")
endOfDay=$(date -u +"%Y-%m-%dT23:59:59Z")

# Function to fetch events for a given calendar ID
fetch_events() {
    local calId=$1
    local url="https://www.googleapis.com/calendar/v3/calendars/${calId}/events?key=${apiKey}&timeMin=${now}&timeMax=${endOfDay}&singleEvents=true&orderBy=startTime"
    
    curl -s "$url" | jq -r '.items[] | 
        (.start.dateTime // .start.date) as $start | 
        if ($start | contains("T")) then
            "\($start[11:16]) - \(.summary)"
        else
            "All day - \(.summary)"
        end'
}

# Fetch and display events for all calendars
for calId in "${calendarIds[@]}"; do
    fetch_events "$calId"
done | sort