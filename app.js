function login() {
    event.preventDefault();
    var username = document.getElementById("username").value;
    var password = document.getElementById("password").value;
    var adminUser = { username: "admin", password: "admin123" };
    var regularUser = { username: "user", password: "user123" };

    if (
      username === adminUser.username &&
      password === adminUser.password
    ) {
      alert("Admin login successful!");
      window.location.href = "admin.html";
    } else if (
      username === regularUser.username &&
      password === regularUser.password
    ) {
      alert("User login successful!");
      window.location.href = "user.html";
    } else {
      alert("Invalid username or password");
    }
  }
  const sign_in_btn = document.querySelector("#sign-in-btn");
  const sign_up_btn = document.querySelector("#sign-up-btn");
  const container = document.querySelector(".container");

  sign_up_btn.addEventListener("click", () => {
    container.classList.add("sign-up-mode");
  });

  sign_in_btn.addEventListener("click", () => {
    container.classList.remove("sign-up-mode");
  });