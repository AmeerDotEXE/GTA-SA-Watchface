module.exports = [
    {
      "type": "heading",
      "defaultValue": "App Configuration"
    },
    {
      "type": "section",
      "items": [
        {
          "type": "heading",
          "defaultValue": "Preference"
        },
        {
          "type": "toggle",
          "messageKey": "IsZoomed",
          "defaultValue": false,
          "label": "Zoom In",
          "description": "Simplified UI With better readability."
        },
        {
          "type": "select",
          "messageKey": "WeekStart",
          "defaultValue": "0",
          "label": "First day in week",
          "options": [
            { 
              "label": "Saterday",
              "value": "2" 
            },
            { 
              "label": "Sunday",
              "value": "1" 
            },
            { 
              "label": "Monday",
              "value": "0" 
            }
          ]
        }
      ]
    },
    {
      "type": "section",
      "items": [
        {
          "type": "heading",
          "defaultValue": "Other"
        },
        {
          "type": "toggle",
          "messageKey": "HasArmour",
          "defaultValue": false,
          "label": "Show Armour",
          "description": "Will not do anything.",
        },
        {
          "type": "toggle",
          "messageKey": "HasBreath",
          "defaultValue": false,
          "label": "Show Breath",
          "description": "Shows steps progress. (Goal: 10k steps)"
        },
        {
          "type": "select",
          "messageKey": "VibrateInterval",
          "defaultValue": "0",
          "label": "Vibrate Interval",
          "options": [
            { 
              "label": "Off",
              "value": "0" 
            },
            { 
              "label": "Every Hour",
              "value": "60" 
            },
            { 
              "label": "Every 30 mins",
              "value": "30" 
            },
            { 
              "label": "Every 20 mins",
              "value": "20" 
            },
            { 
              "label": "Every 15 mins",
              "value": "15" 
            },
            { 
              "label": "Every 12 mins",
              "value": "12" 
            },
            { 
              "label": "Every 10 mins",
              "value": "10" 
            },
            { 
              "label": "Every 1 min",
              "value": "1" 
            }
          ]
        }
      ]
    },
    {
      "type": "submit",
      "defaultValue": "Save Settings"
    }
  ];