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
          "label": "Show Armour"
        },
        {
          "type": "toggle",
          "messageKey": "HasBreath",
          "defaultValue": false,
          "label": "Show Breath"
        },
        {
          "type": "text",
          "defaultValue": "Will not do anything.",
        }
      ]
    },
    {
      "type": "submit",
      "defaultValue": "Save Settings"
    }
  ];