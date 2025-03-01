# 📌 ft_irc - IRC Server Implementation

An implementation of an **IRC (Internet Relay Chat) server** in **C++**, designed to handle multiple clients and provide core IRC functionalities. This project demonstrates **low-level network programming** using the **C++98 standard**, focusing on **socket programming and non-blocking I/O**.

---

## 🚀 Features
- **Handles multiple client connections simultaneously.**
- **Supports the following IRC commands:**
  - `CAP` - Capability negotiation for IRC clients.
  - `INVITE` - Invite a user to a channel.
  - `JOIN` - Join a specified channel.
  - `KICK` - Remove a user from a channel.
  - `MODE` - Change or view a channel's mode.
  - `NICK` - Set or change the nickname of a user.
  - `NOTICE` - Send a notice message to a user or channel.
  - `PART` - Leave a specified channel.
  - `PASS` - Authenticate with a password to connect to the server.
  - `PING` - Server heartbeat mechanism for client connection stability.
  - `PRIVMSG` - Send a private message to a user or channel.
  - `QUIT` - Disconnect from the server.
  - `TOPIC` - Set or view the topic of a channel.
  - `USER` - Set user information upon connecting to the server.
- **Channel Management:**
  - Supports **channel operators and user permissions**.
  - Users can **create, join, and moderate channels**.
- **File Transfer:** Clients can **send documents (files) to each other** using the IRC protocol.
- **Signal Handling:** Gracefully handles system signals (`SIGINT`, `SIGTERM`, etc.).
- **Configurable Server:** Customizable **server port and password**.

---

## 🛠️ Prerequisites
- **C++ Compiler:** Ensure you have a compiler that supports **C++98**.
- **Make:** Used to compile the project.
- **A Unix-based system** (Linux, macOS).

---

## 📝 Installation & Usage
### 📂 1️⃣ Clone the Repository
```sh
git clone https://github.com/karagoz36/ft_irc.git
cd ft_irc
```

### 🔧 2️⃣ Build the Project
```sh
make
```

### 🎮 3️⃣ Run the Server
```sh
./ircserver <port> <password>
```
Example:
```sh
./ircserver 6667 mypassword
```

### 📚 4️⃣ Connect with an IRC Client
To connect to the IRC server, use **any IRC client** such as `irssi`, `WeeChat`, or `HexChat`.

Example using `nc` (netcat):
```sh
nc localhost 6667
```

---

## 📈 Notes
- **Implements RFC 1459** - follows the **Internet Relay Chat (IRC) protocol** specifications.
- **Multi-threaded architecture** for handling multiple users concurrently.
- **Error handling and logging** ensure server stability.

---

