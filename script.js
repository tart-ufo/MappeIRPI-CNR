// Get the modal
var rangeModal = document.getElementById('rangeModal');

// Get the buttons that opens the modals
var rangeBtn = document.getElementById("rangeBtn");

// Get the <span> elements that closes the modals
var rangeSpan = document.getElementsByClassName("rangeSpanClose")[0];

// When the user clicks the button, open the modal 
rangeBtn.onclick = function() {
    rangeModal.style.display = "block";
}

// When the user clicks on <span> (x), close the modal
rangeSpan.onclick = function() {
    rangeModal.style.display = "none";
}

// When the user clicks anywhere outside of the modal, close it
window.onclick = function(event) {
    if(event.target == rangeModal) {
            rangeModal.style.display = "none";
    }
}