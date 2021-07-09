# GW2 Tool Hub

*NOW AVAILABLE*

*OPEN SOURCE*

6/22/21 (Newest Build)
<br>Download Here (v1.2.2): <a href="https://www.dropbox.com/s/zscl0ivxhqh95kr/GW2ToolHub_Installer_1_2_2.zip?dl=1"> .zip Installer </a>

6/20/21 (Old Build)
<br>Download Here (v1.2.1): <a href="https://www.dropbox.com/s/2w0qx9safdr5zh9/GW2ToolHub_Installer_1_2_1.zip?dl=1"> .zip Installer </a>

6/19/21 (Old Build)
<br>Download Here (v1.2.0): <a href="https://www.dropbox.com/s/6ayh1hpn22c7mwh/GW2ToolHub_Installer_1_2_0.zip?dl=1"> .zip Installer </a>

An application aimed at providing resourceful tools to compliment the Guild Wars 2 gaming experience 

# Features

- Adjustable Window Positioning
- Collapsable menus
- Always on top convenience

### Compact Size:
![compact](https://user-images.githubusercontent.com/54217603/123020989-1b2ed880-d388-11eb-8fe4-d74ca8a4a086.png)

### On Demand Material Tracker (with API functionality):
- Search and add materials
- Sort materials by category, material name, current amount, goal amount, and percent complete
- Import excel sheets exported from GW2 Efficiency 
- Set current amount by connecting an API key
- Set the goal amount to reach
- Tabs to keep all your collections organized and separate
![materialTracker](https://user-images.githubusercontent.com/54217603/123021000-2255e680-d388-11eb-8468-78a63f5e1e6e.png)

### Convenient Browser:
- Search all things GW2 at the click of a button
- Play youtube videos to get help with guides
- Easily locate which materials to sell with the help of GW2 Efficiency
- Brush up or refresh on raid knowledge while waiting for a group
- Look up quick crafting guides with GW2 Crafts
![wiki](https://user-images.githubusercontent.com/54217603/123021024-2b46b800-d388-11eb-8104-34321d700ace.png)
![youtube](https://user-images.githubusercontent.com/54217603/123021031-2f72d580-d388-11eb-990c-35808380a748.png)
![GW2E](https://user-images.githubusercontent.com/54217603/123021038-31d52f80-d388-11eb-8e78-663b89685792.png)
![SnowCrows](https://user-images.githubusercontent.com/54217603/123021053-3699e380-d388-11eb-96cf-584979e1d35f.png)
![GW2Crafts](https://user-images.githubusercontent.com/54217603/123021113-5204ee80-d388-11eb-8852-e613d3cfaf0d.png)


### Note Taking Tool:
- Keep all your notes saved and organized with this tool
- Color coded text for extra organization
![Notes](https://user-images.githubusercontent.com/54217603/123021130-5a5d2980-d388-11eb-90ab-3e74b4d31cf1.png)

- and more planned

# Contributors 

### How to Compile, Build, and Run

- Download the 5.15.2 open source Qt build: [Qt Website](https://www.qt.io/download-qt-installer?hsCtaTracking=99d9dd4f-5681-48d2-b096-470725510d34%7C074ddad0-fdef-4e53-8aa8-5e8a876d6ab4)
- During installation make sure to add the following Components: MSVC 2019 64-bit, Qt WebEngine, OpenSSL 1.1.1j Toolkit. If you forget to add the components during installation, you can add them by going to the installation folder and running the 'MaintenanceTool.exe' and follow the wizard. Refer to the following screenshots:

![qt comp](https://user-images.githubusercontent.com/54217603/119823730-2430a500-bec3-11eb-9879-a1d0b88a8a4e.png)

![qt comp1](https://user-images.githubusercontent.com/54217603/119823914-53471680-bec3-11eb-9540-798e914cdd59.png)

- Retrieve the source code
- Open Qt Creator and open the source code (You should be able to either open the GW2_Hub.pro file or open the Qt Creator and open the file from file>Open File> select the GW2_Hub.pro file
- Before you can compile you need to ensure you have the MSVC 2019 compiler set for the project which is one of the components which you added during installation or by using the 'MaintenanceTool.exe'.
- On the left hand side open the 'Projects' Tab
- Disable all other compiles for this project and enable the MSVC2019 64-bit compiler. Refer to the following screenshot:

![qt build](https://user-images.githubusercontent.com/54217603/119825105-9d7cc780-bec4-11eb-9e53-86533c5bad70.png)

- The code should now be able to compile, build, and run

### Important notes

- MSVC2019 compiler is needed in order to compile the webenginewidgets, which is what the QWebEngineView uses. The reason is because Chromium is what is used to power the web widgets and the MSVC2019 compiler has support for Chromium.
- After building the project in debug/release you will get a folder with a name such as: 'build-GW2_Hub-Desktop_Qt_5_15_2_MSVC2019_64bit-Debug' or 'build-GW2_Hub-Desktop_Qt_5_15_2_MSVC2019_64bit-Release'. Inside these folders you will see their respective debug/release folders. In order for the Material Tracker tool to work properly you need to add the 'user.db', 'libcrypto-1_1-x64.dll', and 'libssl-1_1-x64.dll' (Found in the NeededExtras folder) to their respective folders (This only needs to be done once, unless you delete the folders). The 'user.db' is for storing and retrieving all the material data and the two .dll are needed to be able to query the API network See the following screenshots:

![db](https://user-images.githubusercontent.com/54217603/119827553-44626300-bec7-11eb-92dc-27190a72b0a8.png)

![db1](https://user-images.githubusercontent.com/54217603/119827558-46c4bd00-bec7-11eb-9053-d57195d1d56a.png)

# FAQ

- Do I need to sign up for anything to use this application?
    - Zero signing up required.
    
- Is this application free to use?
    - Yes, it is completely free.

- Do I need to be in Window/Windowed Fullscreen to use this?
    - Yes. The application will not work in Fullscreen.
    
- Is this safe to use?
    - Yes. This application does not change or alter anything about the game or its files.
      Any information that is used for data purposes (I.E the Material Tracker Tool)
      comes from available API queries. 

- How do I know this won't give me a virus?
    - I wouldn't jeopardize my reputation nor my ethics with embedding viruses in my application.
      Otherwise you can use services to scan the files for viruses,
      such as: <a href="https://www.virustotal.com/gui/"> www.virustotal.com </a>
      I only host the installer download for my application here on my github page. 
      Please DO NOT download from anywhere else.
      
- Where can I report bugs/issues and/or request features?
    - Here on github: <a href="https://github.com/Michael-R-R/GW2_Hub/issues"> Github Issues </a>

- Why did you create this application?
    - It started as a simple 'Material Goal Tracker' console application example
      that I had used to help teach my significant other C++. I then later on
      decided to take that idea and flesh it out into a GUI application and it 
      quickly became a fun project to continue working on. 

- Do you plan on continuing support and updates for this application?
    - I still have a few ideas and a lot of quality of life 
      improvements that I would like to add. So, yes I do still 
      plan to support and improve this application as long as time allots.
      
- Which platforms are supported?
    - Windows OS
      
- Will this application be perfect and bug free?
    - No, probably not. But, I believe it to be in a stable enough
      condition to finally release to to the public.
     
   Happy Gaming :)
      
