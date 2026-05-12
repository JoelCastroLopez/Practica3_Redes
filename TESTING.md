# Testing with Real Clients

This guide shows how to test your Gopher and Gemini server implementations using real protocol clients, providing a more authentic experience than using `netcat`.

## Why Use Real Clients?

While `netcat` is excellent for debugging and understanding the raw protocol, real clients offer:

- **Visual feedback**: See how your server content is actually rendered
- **Navigation**: Test directory browsing and link following
- **Protocol compliance**: Real clients are stricter about protocol adherence
- **User experience**: Understand how end-users will interact with your server

**When to use each:**
- **netcat**: Debugging, inspecting raw responses, automated testing
- **Real clients**: Manual testing, visual verification, user experience testing

---

## Gopher Clients

### Lynx (Multi-Protocol Terminal Browser)

Lynx is a veteran text-based browser that supports Gopher, HTTP, and other protocols.

#### Installation

```bash
# Debian/Ubuntu
sudo apt install lynx

# Fedora/RHEL
sudo dnf install lynx

# Arch Linux
sudo pacman -S lynx
```

#### Usage

Start your Gopher server:
```bash
cd protocol_server_cpp
./protocol_server gopher 7070
```

In another terminal, connect with Lynx:
```bash
# Browse root directory
lynx gopher://localhost:7070/

# Browse specific file
lynx gopher://localhost:7070/0/test.txt

# Browse subdirectory
lynx gopher://localhost:7070/1/docs
```

**Navigation in Lynx:**
- Arrow keys: Navigate links
- Enter: Follow link
- `q`: Quit
- `g`: Go to URL
- `/`: Search

**What to verify:**
- Directory menus display correctly
- Files open and display their content
- Item types are correct (text files, directories, binaries)
- Navigation between directories works

---

### Bombadillo (Gopher/Gemini Terminal Client)

Bombadillo is a modern terminal client designed specifically for "small internet" protocols like Gopher and Gemini.

#### Installation

```bash
# From source (requires Go)
git clone https://tildegit.org/sloum/bombadillo.git
cd bombadillo
make
sudo make install

# Or check your package manager
# Arch AUR
yay -S bombadillo

# Homebrew (macOS)
brew install bombadillo
```

#### Usage

Start your Gopher server:
```bash
./protocol_server gopher 7070
```

Launch Bombadillo and navigate:
```bash
bombadillo gopher://localhost:7070/
```

**Navigation in Bombadillo:**
- Number keys: Select link by number
- `b`: Go back
- `h`: Home
- `q`: Quit
- `:`: Command mode
- `?`: Help

**Bombadillo features:**
- Vim-like interface
- Supports both Gopher and Gemini
- Bookmarking system
- History navigation
- TLS support for Gemini

**What to verify:**
- Menu items are numbered correctly
- Links work when selected
- Directory listings are complete
- Binary files are handled appropriately

---

### Gopherus (Lightweight Gopher Client)

Gopherus is a minimal, focused Gopher client for the terminal.

#### Installation

```bash
# Requires Python 3
pip3 install gopherus

# Or from source
git clone https://github.com/jamestomasino/gopherus.git
cd gopherus
python3 setup.py install
```

#### Usage

```bash
# Start your server
./protocol_server gopher 7070

# Connect with Gopherus
gopherus gopher://localhost:7070/
```

**Navigation:**
- Number keys: Follow numbered links
- `u`: Go up one directory
- `b`: Go back
- `q`: Quit
- `h`: Help

---

## Gemini Clients

### Lagrange (Graphical Gemini Client)

Lagrange is a beautiful, modern graphical client for Gemini with a clean interface similar to web browsers.

#### Installation

```bash
# Debian/Ubuntu (from PPA or AppImage)
# Download from: https://github.com/skyjake/lagrange/releases

# Arch Linux
sudo pacman -S lagrange

# macOS
brew install --cask lagrange

# Windows
# Download installer from GitHub releases
```

#### Usage

1. Start your Gemini server:
```bash
./protocol_server gemini 1965
```

2. Launch Lagrange (GUI application)

3. In the address bar, type:
```
gemini://localhost:1965/
```

**Features to test:**
- Visual rendering of gemtext
- Image display (if serving images)
- Link navigation
- Directory listings
- Multiple tabs
- Bookmarks

**What to verify:**
- Status codes display correctly (20, 51, 59)
- MIME types are recognized
- Gemtext formatting renders properly
- Links are clickable and work
- Directory listings show all files
- Error messages display for missing files

---

### Amfora (Terminal Gemini Client)

Amfora is a feature-rich terminal client specifically designed for Gemini.

#### Installation

```bash
# Download binary from releases
# https://github.com/makeworld-the-better-one/amfora/releases

# Arch Linux
yay -S amfora

# From source (requires Go)
go install github.com/makeworld-the-better-one/amfora@latest
```

#### Usage

Start your Gemini server:
```bash
./protocol_server gemini 1965
```

Launch Amfora:
```bash
amfora gemini://localhost:1965/
```

**Navigation in Amfora:**
- Number keys: Follow numbered links
- Tab: Switch between tabs
- `Ctrl+T`: New tab
- `Ctrl+W`: Close tab
- `b`: Back
- `q`: Quit
- `Ctrl+R`: Reload
- `:`: Command mode

**Features:**
- Tabbed browsing
- ANSI color support
- Bookmarks
- Subscriptions (RSS/Atom feeds)
- Client certificates
- Theming

**What to verify:**
- Gemtext renders with proper formatting
- Headers (# ## ###) display correctly
- Links are numbered and functional
- Preformatted text blocks display properly
- Lists render correctly
- Status codes are handled appropriately

---

### Bombadillo (Gemini Support)

Bombadillo also supports the Gemini protocol alongside Gopher.

#### Usage for Gemini

```bash
# Start your Gemini server
./protocol_server gemini 1965

# Connect with Bombadillo
bombadillo gemini://localhost:1965/
```

**Testing TLS (Optional):**

If you've implemented TLS support:

```bash
# Start server with TLS
./protocol_server gemini 1965 --tls

# Bombadillo will handle TLS automatically
bombadillo gemini://localhost:1965/
```

**What to verify:**
- Gemtext formatting displays correctly
- Links work
- TLS certificates are accepted (TOFU - Trust On First Use)
- Directory listings render as gemtext
- MIME types are respected

---

## Comprehensive Testing Workflow

### For Gopher Server

1. **Start the server:**
   ```bash
   ./protocol_server gopher 7070
   ```

2. **Test with netcat (raw protocol):**
   ```bash
   echo -e "/\r" | nc localhost 7070
   ```

3. **Test with Lynx (visual verification):**
   ```bash
   lynx gopher://localhost:7070/
   ```

4. **Test with Bombadillo (navigation):**
   ```bash
   bombadillo gopher://localhost:7070/
   ```

5. **Verify:**
   - Root directory menu displays
   - Files can be opened and read
   - Subdirectories can be navigated
   - Item types are correct
   - Error handling works (try non-existent files)

### For Gemini Server

1. **Start the server:**
   ```bash
   ./protocol_server gemini 1965
   ```

2. **Test with netcat (raw protocol):**
   ```bash
   echo -e "gemini://localhost/\r" | nc localhost 1965
   ```

3. **Test with Amfora (terminal):**
   ```bash
   amfora gemini://localhost:1965/
   ```

4. **Test with Lagrange (graphical):**
   - Launch Lagrange
   - Navigate to `gemini://localhost:1965/`

5. **Verify:**
   - Status code 20 for successful requests
   - Correct MIME types in headers
   - Gemtext renders properly
   - Directory listings work
   - Status code 51 for missing files
   - Status code 59 for malformed requests
   - Links are functional

---

## Testing Checklist

### Gopher Protocol

- [ ] Root directory (`/`) displays menu
- [ ] Text files (type 0) display correctly
- [ ] Directories (type 1) show submenu
- [ ] Binary files (type 9) can be downloaded
- [ ] Image files (types g, I) are recognized
- [ ] HTML files (type h) are identified
- [ ] Error items (type 3) for missing files
- [ ] Tab-delimited format is correct
- [ ] Menu ends with `.` on its own line
- [ ] Host and port fields are correct
- [ ] Path traversal attacks are blocked (`../`)

### Gemini Protocol

- [ ] Root URL (`gemini://localhost/`) works
- [ ] Status code 20 for success
- [ ] MIME type `text/gemini` for .gmi files
- [ ] MIME type `text/plain` for .txt files
- [ ] MIME type `application/octet-stream` for binaries
- [ ] Status code 51 for not found
- [ ] Status code 59 for bad requests
- [ ] URLs > 1024 bytes rejected
- [ ] Gemtext headers render correctly
- [ ] Gemtext links work
- [ ] Gemtext lists display properly
- [ ] Directory listings in gemtext format
- [ ] Path traversal attacks are blocked
- [ ] (Optional) TLS encryption works

---

## Troubleshooting

### Common Issues

**Gopher:**
- **Menu items not showing**: Check tab characters (`\t`) between fields
- **Missing terminator**: Ensure menu ends with `.\r\n`
- **Wrong item types**: Verify file extension detection
- **Links don't work**: Check selector paths match filesystem

**Gemini:**
- **"Bad Request" errors**: Verify URL format starts with `gemini://`
- **No content displays**: Check status line format: `20 text/gemini\r\n`
- **Headers not recognized**: Ensure MIME type is correct
- **Links broken**: Verify gemtext link format: `=> /path description`

### Debug Tips

1. **Compare with netcat**: Always verify raw protocol output first
2. **Check server logs**: Look for error messages
3. **Test incrementally**: Start with simple files, then directories
4. **Use multiple clients**: Different clients may reveal different issues
5. **Read the specs**: Refer to RFC 1436 (Gopher) and Gemini spec

---

## Additional Resources

### Gopher
- [RFC 1436 - Gopher Protocol](https://www.rfc-editor.org/rfc/rfc1436)
- [Gopher Client List](https://github.com/prologic/awesome-gopher#clients)
- [Floodgap Gopher Proxy](https://gopher.floodgap.com/gopher/) - Test via web browser

### Gemini
- [Gemini Protocol Specification](https://gemini.circumlunar.space/docs/specification.gmi)
- [Gemini Software List](https://gemini.circumlunar.space/software/)
- [Gemini FAQ](https://gemini.circumlunar.space/docs/faq.gmi)

### Test Servers
- [Gopher Test Server](gopher://gopher.floodgap.com) - Reference implementation
- [Gemini Test Server](gemini://gemini.circumlunar.space) - Official test server

---

## Next Steps

After testing with real clients:

1. **Run automated tests**: Use the provided test suite
2. **Test concurrency**: Open multiple client connections simultaneously
3. **Stress test**: Use the Level 8 tests to verify performance
4. **Security audit**: Verify path traversal prevention works
5. **Code review**: Ensure clean, well-documented code

Good luck with your implementation!
