# QTimeOrganizerWidget
Custom widget based on QWidget Qt framework, inspired by QCalendarWidget

Brief description.

The QTimeOrganizerWidget class provides widget allowing user select a time inteval of the day(s) using mouse. By default, today's date is selected, and the user can select a date a time interval of that day. The currently selected date can be retrieved using the selectedDate() function. The currently selected date can be retrieved using the dateTimeBegin(), dateTimeEnd() functions. Date and time can be set in one go by using setCurrentDate(), setCurrentHour() slots, and the time interval by using setSelectedRange() slot.

The QTimeOrganizerWidget sepate selection day by grid. To set current grid step value using setGrid() function. 
Also the widget has top header allowing change current date by integrated QDateEdit and navigation buttons. Date also can be changed by using nexDate() and previousDate() functions.

When time interval change it emits signal rangeChanged(), and when date changed it emits signal dateChanged().
You can set tracking of current selection by setTracking(). When tracking is enabled widget emits signal immideatly when selection interval changed.

The currently displayed date can be retrieved using the currentDate().
The current selected time interval by dateTimeBegin(), dateTimeEnd() functions. 
