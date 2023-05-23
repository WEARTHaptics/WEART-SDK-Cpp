$(document).ready(function () {
  const modal = document.getElementById("myModal");
  const modalImg = document.getElementById("img01");
  const captionText = document.getElementById("caption");

  // Select all pictures inside the contents section
  var images = document.querySelectorAll(".contents img");

  images.forEach(function (img) {
    img.onclick = function () {
      // When a picture is clicked, copy it in the modal and show it
      modal.style.display = "block";
      modalImg.src = this.src;
      modalImg.alt = this.alt;
      captionText.innerHTML = this.alt;
    };
  });

  // When the user clicks on the modal, close it
  modal.onclick = function () {
    img01.className += " out";
    setTimeout(function () {
      modal.style.display = "none";
      img01.className = "modal-content";
    }, 400);
  };
});
