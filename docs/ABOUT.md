# Medos

## Abstract
Medos is a lightweight and efficient video playback application designed for simplicity, performance, and accessibility. It offers essential playback features, such as play, pause, and fast-forward, while integrating advanced functionalities like simple video editing, auto-generated captions, and subtitle translation. With a focus on cross-platform compatibility, this application is designed to enhance user experience through a clean interface and seamless usability. By including stretch goals like integration with video-sharing APIs, the media player aspires to be a versatile tool for both casual users and creators.

## Goal
The primary goal is to create a simple yet robust desktop video player that balances functionality and user-friendliness. By eliminating unnecessary complexities found in traditional players, the project aims to deliver a streamlined tool that focuses on playback efficiency, basic editing, and accessibility enhancements. The project also seeks to provide users with a consistent and familiar interface across platforms, further broadening its usability and appeal.

## Current Practice
Most existing video players are burdened with excessive features that overwhelm users and contribute to bloated performance. These tools often lack cross-platform consistency, requiring users to learn multiple interfaces across devices. Additionally, basic video editing is often relegated to specialized software, leaving a gap for casual users who require simple, intuitive editing tools.

## Novelty
Medos stands out by blending core video playback functionality with accessibility-focused features and lightweight design. Unlike traditional video players that prioritize extensive but often cumbersome feature sets, this application emphasizes simplicity while introducing essential tools for modern media use. Auto-generated captions and real-time subtitle translation enhance accessibility, making the player inclusive for users with hearing impairments or language barriers. The inclusion of basic video editing tools, such as trimming and stitching, empowers users to create and customize content without the steep learning curve of complex editing software.

Additionally, the player’s stretch goals elevate its value as a versatile media hub. Cross-platform support ensures a seamless user experience across multiple devices, while integration with video-sharing APIs (e.g., YouTube, Twitch, Vimeo) offers convenient options for uploading, downloading, and streaming media. By combining these practical and creative features, Medos transforms from a standard playback tool into a comprehensive solution for everyday media consumption and lightweight content creation.

## Effects
A successful implementation will redefine how users interact with video playback and editing tools. By focusing on user experience and accessibility, this project aims to lower the barrier to entry for video editing and provide a seamless playback experience. Furthermore, features like subtitle translation and API integration will expand the player’s usability in global and collaborative contexts, benefiting both casual viewers and content creators.

---

## Non-Functional and External Requirements
### Non-Functional Requirements:
1. The application must have a responsive and intuitive GUI that ensures ease of navigation.
2. The video player must operate efficiently on a wide range of hardware, including older systems, without significant performance degradation.
3. Playback and editing processes should run smoothly with minimal latency or resource overhead.

### External Requirements:
1. The application must handle invalid inputs gracefully, such as unsupported video formats, to ensure robustness.
2. It must be easy to install, run, and maintain, with detailed documentation for developers and end-users.
3. Developers must be able to build the software from source, facilitating the addition of new features.
4. Each primary feature and use case must be achievable within the project’s timeframe, adhering to available resources and skills.

---

## Technical Approach  
- **Languages**: The application is built with **C++** for high performance and memory efficiency.  
- **Frameworks and Libraries**:  
  - **FFmpeg** for robust video/audio decoding and processing.  
  - **ImGui** for a lightweight and responsive graphical interface.  
  - **Vulkan API** for high-performance video and GUI rendering.  
  - **Whisper ASR Library** for real-time automatic speech recognition and translations.  
  - **SDL3** for cross-platform audio output.  
- **Tools**:  
  - **Git & CMake** streamline development and build processes.  
  - **GitHub Actions** enables a simple **CI/CD pipeline** for automated builds and testing.

The technical implementation focuses on modular design, allowing flexibility for future feature additions and performance optimizations.

---

## Risks, Team Information, and Timeline
### Risks and Mitigation
1. **Library Familiarity**: Some team members are unfamiliar with FFmpeg and ImGui.
    - **Likelyhood**: High
    - **Mitigation**: Allocate time for learning resources and prototyping early in the project.
2. **Coordination Challenges**: With a large team, maintaining consistent coding standards may be difficult.
    - **Likelyhood**: Medium
    - **Mitigation**: Use detailed style guides and conduct weekly code reviews.
3. **Time Constraints**: Balancing project work with coursework may lead to delays.
    - **Likelyhood**: High
    - **Mitigation**: Set clear milestones and prioritize high-impact tasks.
4. **Familiarity with Build System**: Some members are unfamiliar with Cmake. 
    - **Likelyhood**: Medium
    - **Mitigation**: Members with Cmake expirience will provide examples of Cmake structure and assistance.
5. **Delayed task completion due to workload or unforseen complications**: Tasks may not completed on time due to unforseen problems. Due to the time constraint above this needs to be handled quickly. 
    - **Likelyhood**: Medium
    - **Mitigation**: Establish consistent communication through meeting to ensure members can complete on time. Redistribute work when problems arise. 

## Team Members and Roles

| **Team Member**  | **Role**           |  
|------------------|--------------------|  
| Hunter McCoy     | Backend Developer  |  
| Quinton Gonzales | Tester             |  
| Jared Northrop   | Backend Developer  |  
| Alexander Martin | Backend Developer  |  
| Thai Le          | Tester             |
| Reynaud Hunter   | Frontend Developer |

### Reasoning for Roles

The project’s structure is designed to maximize efficiency and ensure high-quality development, given the scope and complexity of creating a desktop media player.

- **Backend Developers (3)**: The decision to have three backend developers is essential for creating a solid and performant application. Since the core functionality of the media player (such as video decoding, playback control, and subtitle processing) is handled in the backend, it’s crucial to have sufficient resources dedicated to these complex tasks. The backend team will ensure that the application’s foundation is reliable, scalable, and optimized. Their work will include integrating libraries like FFmpeg and handling multithreading, file management, and performance optimizations.

- **Frontend Developer (1)**: With one dedicated frontend developer, we ensure a unified and cohesive user interface (UI) design. This role focuses on translating the backend functionality into an intuitive and aesthetically pleasing UI, making the player accessible and easy to use. The frontend developer will collaborate closely with backend developers to ensure that the user interface interacts seamlessly with the application’s core features. Having one frontend developer ensures a consistent design vision while minimizing the back-and-forth between team members, promoting efficiency and clarity in UI decisions.

- **Testers (2)**: Testing is critical for ensuring the reliability and usability of the application. The two testers will work collaboratively to conduct thorough testing of the player’s features, including unit, integration, performance, and user acceptance testing. They will be responsible for identifying bugs and ensuring that the project requirements are met at every stage of development. With a dedicated testing team, feedback loops are shortened, and both frontend and backend developers can quickly address issues as they arise. Testers will also ensure that the application meets cross-platform expectations and that the user experience is smooth and intuitive.

This structure balances the need for robust backend development with a focused frontend vision and comprehensive testing, ensuring that the application’s performance, usability, and functionality are all top-notch.

### Fit to Expertise
The roles were carefully assigned based on the individual team members’ expertise and prior experience to ensure that each member is contributing in an area they excel in. Backend developers were selected for their strong understanding of performance optimization, API integration, and media processing, while the frontend developer was chosen for their proficiency in designing intuitive user interfaces that align with the app’s simplicity goals. The testers bring a keen eye for detail and a deep understanding of quality assurance practices, ensuring that the app is both stable and user-friendly. This approach leverages the team’s strengths, allowing each member to contribute effectively to the project’s success.

## Development Timeline

| **Week(s)** | **Focus Area**                                         |  
|------------|--------------------------------------------------------|  
| **Weeks 2–3** | Research and setup                                     |  
| **Weeks 4–6** | Implement MVP features and basic GUI                   |  
| **Weeks 7–8** | Test MVP and begin implementing stretch goals          |  
| **Week 9** | Finalize features, optimize performance, and prepare documentation |

## Sub-group Development Timeline

### Backend Developers
- **Weeks 2–3**:
    - **Milestone 1**: Set up development environment and integrate FFmpeg library for video decoding.
    - **Milestone 2**: Define backend architecture and modular design (e.g., separate modules for playback, subtitle processing, and video file management).
    - **Milestone 3**: Complete initial implementation of basic video decoding and playback functionality.

- **Weeks 4–6**:
    - **Milestone 4**: Implement core features, such as video decoding, playback control, and subtitle support.
    - **Milestone 5**: Ensure video playback synchronizes properly with subtitles (including auto-generated captions).
    - **Milestone 6**: Test basic video playback functionality on at least two different file types.

- **Weeks 7–8**:
    - **Milestone 7**: Implement stretch goals, such as video editing or transcoding features (e.g., trim, rotate, etc.).
    - **Milestone 8**: Optimize multithreading for video decoding and performance enhancements (e.g., load time improvements).
    - **Milestone 9**: Complete integration of backend components with frontend for seamless UI interaction.

- **Week 9**:
    - **Milestone 10**: Finalize backend features and optimize for performance.
    - **Milestone 11**: Complete backend testing (unit tests and performance testing), ensuring all components are error-free and stable.
    - **Milestone 12**: Provide final integration support for the frontend team.

### Frontend Developer
- **Weeks 2–3**:
    - **Milestone 1**: Set up ImGui for UI development and create initial wireframes and UI mockups.
    - **Milestone 2**: Implement basic layout of the user interface, including video display and control buttons (play, pause, stop, etc.).
    - **Milestone 3**: Collaborate with backend developers to ensure seamless video rendering from the backend.

- **Weeks 4–6**:
    - **Milestone 4**: Develop core UI components for video playback controls, including pause, resume, volume control, and timeline.
    - **Milestone 5**: Implement a video file explorer and ensure that it integrates with the backend for file loading.
    - **Milestone 6**: Complete basic styling and responsive design for desktop and mobile layouts.

- **Weeks 7–8**:
    - **Milestone 7**: Integrate additional features such as video quality settings, subtitle toggle, and playback speed control.
    - **Milestone 8**: Implement accessibility features such as color contrast adjustments, and user preference storage.
    - **Milestone 9**: Begin testing with real user data, ensuring the UI is intuitive and responsive on various screen sizes.

- **Week 9**:
    - **Milestone 10**: Finalize UI design and fix any UI-related bugs.
    - **Milestone 11**: Ensure full integration with the backend (e.g., ensure video playback controls and subtitle syncing are functioning properly).
    - **Milestone 12**: Conduct user acceptance testing (UAT) to ensure the UI meets end-user expectations.

### Testers
- **Weeks 2–3**:
    - **Milestone 1**: Set up test environments on multiple platforms (Windows, Mac, Linux) and configure CI/CD pipelines for automated testing.
    - **Milestone 2**: Define test cases and scenarios based on project requirements, such as video playback, subtitle synchronization, and basic UI interactions.

- **Weeks 4–6**:
    - **Milestone 3**: Perform initial testing of the video decoding, playback control, and subtitle features.
    - **Milestone 4**: Execute integration testing between frontend and backend, ensuring seamless interaction between UI and video playback functions.
    - **Milestone 5**: Document and report bugs, ensuring each issue is tracked and prioritized for resolution.

- **Weeks 7–8**:
    - **Milestone 6**: Conduct performance and load testing (e.g., video file size handling, multiple subtitle formats, various video resolutions).
    - **Milestone 7**: Test cross-platform compatibility, ensuring the app functions properly across all targeted operating systems.
    - **Milestone 8**: Verify all stretch goals are functioning correctly (e.g., video editing features, subtitle translation).

- **Week 9**:
    - **Milestone 9**: Perform final regression testing to ensure no new issues have been introduced.
    - **Milestone 10**: Finalize test documentation, provide feedback to frontend and backend teams on any last-minute issues.
    - **Milestone 11**: Ensure all project requirements are met, including usability, stability, and cross-platform compatibility.


### Soliciting Feedback
External feedback will be most useful during weeks 5-6 when the MVP is completed. At this stage, we will have implemented the core features of the app, such as video playback, subtitle integration, and basic UI functionality. Soliciting feedback from peers, friends, and potential users will allow us to identify usability issues, improve user experience, and ensure the app meets the expectations of a diverse user base. We will gather feedback through structured user testing sessions, surveys, and informal reviews to assess how well the app performs in real-world scenarios. This feedback will be invaluable for refining the app's design, identifying any overlooked bugs, and ensuring that the final product aligns with user needs before moving into final development and optimization stages.

## Communication

### Communication Guidelines
1. **Professionalism**: Maintain respectful, clear, and concise communication in all interactions.
2. **Response Time**: Respond to messages and requests within 24 hours on business days. If additional time is needed, provide an update.
3. **Proactive Communication**:
    - Notify the team promptly if you anticipate being unavailable or delayed.
    - Share progress updates regularly to ensure alignment and visibility.
4. **Issue Reporting**: Raise challenges or blockers as soon as they arise to minimize project delays.
5. **Meeting Etiquette**:
    - Arrive prepared and on time for all scheduled meetings.
    - Stick to the agenda and respect allocated discussion times.
6. **Documentation**: Summarize key decisions and action items after discussions and post them in the shared communication channel.


### Communication Channels
- **Discord**: For daily communication, quick updates, and team collaboration.
- **Email**: Used for formal updates, important announcements, and sharing detailed documents.

### Meeting Schedule
- **Weekly Team Meeting**: Fridays at 9:45 AM with the TA.
    - Purpose: Progress updates, addressing challenges, and clarifying objectives.
    - Platform: In-Person, BEXL 322 or an agreed alternative (e.g., Zoom).
- **Stand-Ups**: Mondays and Wednesdays at 5:40 PM (immediately after class).
- **Ad-Hoc Meetings**: Scheduled as needed for urgent issues or significant milestones.

## Features

### Major Features (MVP)

1. **Video Playback**: Seamless playback of common video formats (e.g., MP4, MKV, AVI).
2. **Playback Controls**: Play, pause, fast-forward, rewind, and seek functionality.
3. **Custom Playback Speed**: Adjust video playback speed dynamically (e.g., 0.5x, 1x, 2x).
4. **Closed Captioning**: Display synchronized captions during video playback.
5. **Subtitle Translation**: Translate subtitles to different languages in real time.
6. **Quick Edits**: Trim, stitch, and make basic edits to video files within the application.
7. **Playlist Support**: Create, save, and play video playlists with customizable playback order.
8. **Audio Transcription**: Generate text transcriptions from video or audio files.

### Stretch Goals

1. **Cross-Platform Compatibility**: Develop the application for Windows, macOS, and Linux.
2. **Video Sharing API Integration**: Enable uploading, downloading, and streaming via third-party services like YouTube or Vimeo.
3. **Advanced Video Editing**: Add features such as cropping, color adjustments, and video effects.
4. **Cloud Integration**: Allow users to sync playlists and video libraries with cloud services.
5. **Hardware Acceleration**: Optimize video playback using GPU acceleration for improved performance.
6. **Theme Customization**: Provide multiple themes (e.g., dark mode, high-contrast mode) for better accessibility.

## Use Cases

### 1. **Audio Transcribing**
**Actor**: Product user  
**Trigger**: User selects an audio file or a video with audio for transcription.  
**Preconditions**: The user has imported an audio or video file into the application.  
**Postconditions (success scenario)**: A text document containing the transcribed audio is saved or displayed.  
**Steps (success scenario)**:
1. User navigates to the transcription feature.
2. User selects an audio or video file.
3. The application processes the file and generates a transcription.
4. User reviews and edits the transcription if needed.
5. User saves the transcription as a text file.

**Extensions**: User cancels the transcription process.  
**Exceptions**:
- The file format is unsupported.
- The transcription process fails due to poor audio quality or missing data.

---

### 2. **Closed Captioning**
**Actor**: Product user  
**Trigger**: User plays a video and enables the closed captioning feature.  
**Preconditions**: The video file supports audio and is loaded into the application.  
**Postconditions (success scenario)**: The application displays synchronized captions during playback.  
**Steps (success scenario)**:
1. User selects a video file to play.
2. User enables closed captioning.
3. The application generates or loads captions and synchronizes them with playback.
4. User adjusts caption settings (e.g., font size, position).

**Extensions**:
- User modifies the caption file directly.
- User disables closed captioning mid-playback.

**Exceptions**:
- Captions fail to load or generate.
- The user cannot adjust caption settings.

---

### 3. **Software Hub for Various APIs**
**Actor**: Product user  
**Trigger**: User integrates third-party video-sharing services into the application.  
**Preconditions**: The application supports the API of the selected service.  
**Postconditions (success scenario)**: The user successfully uploads, downloads, or streams video content using the integrated API.  
**Steps (success scenario)**:
1. User navigates to the API integration menu.
2. User selects a service to integrate.
3. User authenticates with the third-party service.
4. The application displays the integrated features (e.g., upload/download buttons).
5. User interacts with the service via the application.

**Extensions**: User removes or switches API integrations.  
**Exceptions**:
- Authentication fails.
- API limitations or errors prevent successful interaction.

---

### 4. **Playlist Integration and Support**
**Actor**: Product user  
**Trigger**: User creates or imports a playlist.  
**Preconditions**: The user has video files accessible for adding to a playlist.  
**Postconditions (success scenario)**: The application plays the playlist in order, with options for customization.  
**Steps (success scenario)**:
1. User navigates to the playlist feature.
2. User creates a new playlist or imports an existing one.
3. User adds video files to the playlist.
4. User customizes playback order or options.
5. The playlist plays back as configured.

**Extensions**:
- User rearranges playlist order during playback.
- User removes a file from the playlist.

**Exceptions**:
- File format of an added video is unsupported.
- The playlist fails to save or load.

---

### 5. **Custom Playback Speed Adjustment**
**Actor**: Product user  
**Trigger**: User changes the playback speed of the current video.  
**Preconditions**: A video file is loaded and playing.  
**Postconditions (success scenario)**: The video plays at the adjusted speed without interruptions.  
**Steps (success scenario)**:
1. User opens the playback settings menu.
2. User adjusts the speed using a slider or preset options (e.g., 0.5x, 1.0x, 2.0x).
3. The application adjusts playback speed dynamically.

**Extensions**:
- User resets to the default playback speed.
- User saves a custom speed for future use.

**Exceptions**:
- Playback becomes choppy due to hardware limitations.
- Slider fails to adjust the speed accurately.

---

### 6. **Quick Edits**
**Actor**: Product user  
**Trigger**: User trims or edits a video directly within the application.  
**Preconditions**: A video file is loaded into the application.  
**Postconditions (success scenario)**: The edited video is saved as a new file.  
**Steps (success scenario)**:
1. User enters the edit view.
2. User selects start and end points to trim.
3. User applies the edit and previews the result.
4. User saves the edited video to their desired location.

**Extensions**:
- User cancels the editing process.
- User performs additional edits (e.g., cropping or adding effects).

**Exceptions**:
- Video fails to save.
- The application crashes during editing.

## Software Architecture
**User Interface**:
The UI needs to provide simple and clear options to the user to interact with the media player. Videos need to render and playback smoothly. 
- The UI links user actions with the libraries used perform all the features and specifications of the application. 

**Backend Libraries**:
Libraries are used to facilitate the core functionalities of application. Each implemented library focuses on a single functionality of the application, creating a modular design. 
- The libraries are called by the UI to implement their functionalities. Libraries also link to third party or standard libraries to optimally perform complex operations. 

## Software Design
**User Interface**:
The UI wil be implemented through a the IMGUI library. This component is responsible for collecting the users actions and displaying the results to the user. 

**Backend Libraries**:
The backend libaries provide the functionality of the application. They integrate functions provided by third party and standard libraries to fit the needs of the media player as well as create the workflow between operations. Each backend library is defined by a core feature of the media player creating a modular design where each feature works independently. The third party libraries include IMGUI, FFMPeg, Whisper ASR CPP, and Vulkan SDK.


## Coding guidlines
The C++ Core Guidlines will be used to create a constistent coding style between project members. These are common guidelines widely used and adopted in the world. The guidelines specify ways to enforce their rules through several options: static analysis, compiler, run-time checks, and human review. All pull request to the main branch require approval from another member, ensuring multiple sets of eyes have looked over the code for consistent style and best practices. 

## Conclusion
Medos combines innovative features with a user-centric approach to create a streamlined, reliable video playback and editing tool. By prioritizing simplicity, accessibility, and performance, this project aims to fill the gap between complex professional tools and basic video players. With a strong focus on modularity and cross-platform capabilities, the player offers an adaptable solution for modern media consumption and creation.

## Reflections

### Hunter
- I would definately spend more time thinking of project ideas, and try a much easier one. Creating a media player from scratch was incredibly difficult.
- I would be more forceful about getting team members to communicate about what they are doing for the project, where they're at with it, and any issues they are having. This also applies to getting them to start working on it earlier.
- I would choose more feasible use cases, and add to them if I feel we are capable of doing more.

### Quinton
- The scope of the project seemed very doable and I was very excited to implement all of the interesting and unique features that our proposal contained. Keeping up with the milestones due to the larger scope proved far more difficult than I imagined. I need to realize what can be done realistically in 10 weeks.
- In the future, I also plan to "scrap" features sooner than later. I ended up taking too long on a feature, just to give up on it closer to the finish line than I would have liked. 
- I hope to implement better communications plans. While I feel like I have been communicating very consistently, I would like to plan to ask each team member what their task/goal is for the week, so the team knows who is doing what.
- Taking this course while also taking 14 other credits felt like it better prepared me for future software engineering careers. While I felt a little unprepared this term, I now know how it feels to have several larger scoped projects with short deadlines.

### Jared
- I think the project had close to the proper scope for seven people with uniform participation. However this team did not function at the level required to fulfull all features we planned to implement in the 9 weeks. 
- One of the things I saw during this project was the need of a leader to ensure consistnent updates of individual project updates. There was low communication which made it unclear on the progress of various members. 
- This project showed how poor communication slows down a project and can put a lot or pressure on certain members if work load has to be redistrubuted due to a members work not being completed on time. 
- One of the biggest techinical ideas I learned the most about, was how build systems work to create complete applications. Previously I had not done much with build systems and this project allowed me to use and get used to them. 

### Reynaud
- I should have asked for help sooner instead of trying to figure everything out on my own. It would have saved time and reduced stress.
- Better communication with my team was needed. I should have checked in more often instead of working independently.
-	I need to prioritize teamwork—coordinating with others would have made the project smoother.
-	I should have been more proactive in tracking team progress and addressing issues earlier.
-	Moving forward, I will communicate more, collaborate better, and seek help when needed.

### Alexander
- I would definitely have a focus on early communication. The start of this project was extremely slow which led to a lot of work happening later on and team members being disconnected. If we were to have established better roles and communication guidelines for all team members at the start as well as expectations, that would have helped us be more successful as a team. Making sure everyone understood the project's big picture, build system, and their own responsibilities would have gone a long way towards ensuring a successful project.
- I learned a lot about GitHub Actions and creating cross-platform desktop applications. This project provided me with the foundational understanding of what is necessary to both build source code and distribute binary applications across multiple different operating systems and the cloud. Setting up automatic releases on GitHub was interesting to accomplish as well, but if I were to do it again, I would get the release pipeline set up at the same time as the CI pipeline since there was definitely some debugging done quite late just to get it running cross-platform.
- I would have spent much more time in the planning and research phase. Although code wasn't started immediately, it felt like there was a lot of pressure to get something together. However, if we had spent more time researching all the details and coming up with a better plan to bring all the components into one application, it would have saved us time in the long run. Several times our libraries had to have their public interface modified because something wasn't thought through well enough. With the scope of this project that wasn't the biggest problem ever, but I could see it being a much larger problem if this application was developed with more people or over a longer timespan.

### Thai
- I should've definitely be more involved with my team instead of relying on them so that was my fault.
-	I feel like I should've communicated more with TA's and my team about help.
-	I feel like my lack of experience in making projects like this was what hindered me and my team's progress
