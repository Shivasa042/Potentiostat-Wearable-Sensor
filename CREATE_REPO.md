# 🔧 Create GitHub Repository - Step by Step

Your local repository is ready, but the GitHub repository needs to be created first. Follow these steps:

## Step 1: Create Repository on GitHub

1. Go to **https://github.com/new**

2. Fill in the repository details:
   - **Repository name**: `HELPStat`
   - **Description**: `Wearable Electrochemical Potentiostat with ESP32-S3 and AD5940`
   - **Visibility**: Select **Public** (to share with others) or **Private** (team only)
   - **Initialize**: Leave **unchecked** ✗ (Don't add README, .gitignore, or license - we have local commits)

3. Click **Create repository** button

4. You'll see a page with setup instructions. Look for the section titled "...or push an existing repository from the command line"

---

## Step 2: Copy Your Personal Access Token

Since GitHub is moving away from password authentication:

1. Go to **GitHub Settings** → **Developer settings** → **Personal access tokens** → **Tokens (classic)**
   - Direct URL: https://github.com/settings/tokens

2. Click **Generate new token (classic)**

3. Fill in:
   - **Note**: `HELPStat Push`
   - **Expiration**: 30 days (or per your preference)
   - **Scopes**: Check `repo` (this gives full control of repositories)

4. Click **Generate token**

5. **Copy the token** (it will only display once!) - it looks like `ghp_XXXXXXXXXXX`

---

## Step 3: Push Code to GitHub

Run this command:

```powershell
cd "c:\Users\Shiva\OneDrive\Documents\UC Berkeley MEng\Capstone\Wearable Sensor\HELPStat-main\HELPStat"
git push -u origin main
```

When prompted for password:
- **Username**: `Shivasa042`
- **Password**: Paste your Personal Access Token (NOT your GitHub password)

---

## 🎯 Alternative: Use SSH Keys (Advanced)

If you prefer SSH authentication:

1. Check if you have SSH keys:
   ```powershell
   Test-Path $env:USERPROFILE\.ssh\id_rsa
   ```

2. If not, generate keys:
   ```powershell
   ssh-keygen -t rsa -b 4096 -f "$env:USERPROFILE\.ssh\id_rsa"
   # Press Enter for all prompts
   ```

3. Add public key to GitHub:
   - Go to https://github.com/settings/keys
   - Click **New SSH key**
   - Paste contents of `.ssh\id_rsa.pub`

4. Change remote to SSH:
   ```powershell
   git remote set-url origin git@github.com:Shivasa042/HELPStat.git
   git push -u origin main
   ```

---

## ✅ Verify Success

After pushing, you should see:
```
Enumerating objects: 101, done.
Counting objects: 100% done.
Writing objects: 100% done.
Total 101 (delta 0), reused 0 (delta 0)
To https://github.com/Shivasa042/HELPStat.git
 * [new branch]      main -> main
Branch 'main' set up to track remote branch 'main' from 'origin'.
```

Then visit **https://github.com/Shivasa042/HELPStat** to see your project live!

---

## 🆘 Troubleshooting

### "fatal: remote: Repository not found"
- Confirm repository exists on GitHub.com
- Check username spelling (case-sensitive in some cases)
- Verify visibility settings (public/private)

### "fatal: Authentication failed"
- Confirm Personal Access Token was copied correctly
- PAT must have `repo` scope
- Try generating a new token

### "Updates were rejected"
```powershell
git pull origin main --allow-unrelated-histories
git push -u origin main
```

---

**Ready? Follow the steps above and your HELPStat project will be on GitHub! 🚀**
