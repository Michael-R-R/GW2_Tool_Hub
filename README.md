# GW2 Tool Hub

*NOW AVAILABLE*

*Open source coming soon - Stay tuned*

Download Here: <a href="https://www.dropbox.com/s/2cbs88b3kfnpm61/GW2ToolHub_Installer.zip?dl=1"> .zip Installer </a>

An application aimed at providing resourceful tools to compliment the Guild Wars 2 gaming experience 

### Features

- Adjustable Window Positioning
- Collapsable menus
- Always on top convenience

- Compact Size:
![GW2Hub_01](https://user-images.githubusercontent.com/54217603/119273426-ebe64980-bbd8-11eb-8a0f-1f8f65b21cdb.jpg)

- On Demand Material Tracker (with API functionality):
    - Search and add materials
    - Set current amount by connecting an API key
    - Set the goal amount to reach
    - Tabs to keep all your collections organized and separate
![GW2Hub_03](https://user-images.githubusercontent.com/54217603/119273429-eee13a00-bbd8-11eb-8028-bd65e2aab484.png)

- Convenient Browser:
    - Search all things GW2 at the click of a button
    - Play youtube videos to get help with guides
    - Easily locate which materials to sell with the help of GW2 Efficiency
    - Brush up or refresh on raid knowledge while waiting for a group
![GW2Hub_02](https://user-images.githubusercontent.com/54217603/119273428-ed177680-bbd8-11eb-84bf-639f4985166e.png)
![GW2Hub_05](https://user-images.githubusercontent.com/54217603/119273419-e7219580-bbd8-11eb-9894-5006597b0372.png)
![GW2Hub_06](https://user-images.githubusercontent.com/54217603/119273421-e8eb5900-bbd8-11eb-9a19-bf89de8ae788.png)
![GW2Hub_07](https://user-images.githubusercontent.com/54217603/119273423-ea1c8600-bbd8-11eb-9690-ef28c54c27e7.png)

- Note Taking Tool:
    - Keep all your notes saved and organized with this tool
    - Color coded text for extra organization
![GW2Hub_04](https://user-images.githubusercontent.com/54217603/119273416-e557d200-bbd8-11eb-8559-4031fb4d0738.png)

- and more planned

# Contributers

### How to Compile, Build, and Run

- Download the 5.15.2 open source Qt build: [Qt Website](https://www.qt.io/download-qt-installer?hsCtaTracking=99d9dd4f-5681-48d2-b096-470725510d34%7C074ddad0-fdef-4e53-8aa8-5e8a876d6ab4)
- During installation make sure to add the following Components: MSVC 2019 64-bit, Qt WebEngine, OpenSSL 1.1.1j Toolkit. If you forget to add the components during installation, you can add them by going to the installation folder and runnign the 'MaintenanceTool.exe' and follow the wizard. Refer to the following screenshots:
![qt comp](https://user-images.githubusercontent.com/54217603/119823730-2430a500-bec3-11eb-9879-a1d0b88a8a4e.png)
![qt comp1](https://user-images.githubusercontent.com/54217603/119823914-53471680-bec3-11eb-9540-798e914cdd59.png)

- Retrieve the source code
- Open Qt Creator and open the source code (You should be able to either open the GW2_Hub.pro file or open the Creator and open the file from file>Open File> select the GW2_Hub.pro file
- Before you can compile you need to ensure you have the MSVC 2019 compiler set for the project which is one of the components which you added during installation or by using the 'MaintenanceTool.exe'.
- On the left hand side open the 'Projects' Tab
- Disable all other compiles for this project and enable the MSVC2019 64-bit compiler. Refer to the following screenshot:
![qt build](https://user-images.githubusercontent.com/54217603/119825105-9d7cc780-bec4-11eb-9e53-86533c5bad70.png)

- The code should now be abile to compile, build, and run

### Important notes


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
      
