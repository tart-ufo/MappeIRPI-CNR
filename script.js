// Get the modals
var dayModal = document.getElementById('dayModal');
var rangeModal = document.getElementById('rangeModal');

// Get the buttons that opens the modals
var DayBtn = document.getElementById("dayBtn");
var rangeBtn = document.getElementById("rangeBtn");

// Get the <span> elements that closes the modals
var daySpan = document.getElementsByClassName("daySpanClose")[0];
var rangeSpan = document.getElementsByClassName("rangeSpanClose")[0];

// When the user clicks the button, open the modal 
DayBtn.onclick = function() {
    dayModal.style.display = "block";
}
rangeBtn.onclick = function() {
    rangeModal.style.display = "block";
}

// When the user clicks on <span> (x), close the modal
daySpan.onclick = function() {
    dayModal.style.display = "none";
}
rangeSpan.onclick = function() {
    rangeModal.style.display = "none";
}

// When the user clicks anywhere outside of the modal, close it
window.onclick = function(event) {
    switch (event.target) {
        case dayModal:
            dayModal.style.display = "none";
            break;
        case rangeModal:
            rangeModal.style.display = "none";
            break;
    }
}