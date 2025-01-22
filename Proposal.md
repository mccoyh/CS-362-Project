# Project Proposal

**GitHub Repository**: [https://github.com/mccoyh/CS-362-Project](https://github.com/mccoyh/CS-362-Project)

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

## Team Members and Roles

| **Team Member**  | **Role**  |  
|------------------|-----------|  
| Hunter McCoy     | Developer |  
| Quinton Gonzales | Developer |  
| Jared Northrop   | Developer |  
| Alexander Martin | Developer |  
| Thai Le          | Developer |
| Reynaud Hunter   | Developer |

## Product Description

### Title: Desktop Media Player

## Abstract
The Desktop Media Player is a lightweight and efficient video playback application designed for simplicity, performance, and accessibility. It offers essential playback features, such as play, pause, and fast-forward, while integrating advanced functionalities like simple video editing, auto-generated captions, and subtitle translation. With a focus on cross-platform compatibility, this application is designed to enhance user experience through a clean interface and seamless usability. By including stretch goals like integration with video-sharing APIs, the media player aspires to be a versatile tool for both casual users and creators.

## Goal
The primary goal is to create a simple yet robust desktop video player that balances functionality and user-friendliness. By eliminating unnecessary complexities found in traditional players, the project aims to deliver a streamlined tool that focuses on playback efficiency, basic editing, and accessibility enhancements. The project also seeks to provide users with a consistent and familiar interface across platforms, further broadening its usability and appeal.

## Current Practice
Most existing video players are burdened with excessive features that overwhelm users and contribute to bloated performance. These tools often lack cross-platform consistency, requiring users to learn multiple interfaces across devices. Additionally, basic video editing is often relegated to specialized software, leaving a gap for casual users who require simple, intuitive editing tools.

## Novelty
The Desktop Media Player introduces a unique combination of features aimed at accessibility, simplicity, and cross-platform support. While most video players focus solely on playback, this application extends functionality with integrated video editing tools, auto-generated captions, and subtitle translation to cater to diverse user needs. The stretch goals of cross-platform functionality and API integration for video sharing position the player as both a practical tool and a creative hub for modern media consumption.

## Effects
A successful implementation will redefine how users interact with video playback and editing tools. By focusing on user experience and accessibility, this project aims to lower the barrier to entry for video editing and provide a seamless playback experience. Furthermore, features like subtitle translation and API integration will expand the player’s usability in global and collaborative contexts, benefiting both casual viewers and content creators.

---

## Technical Approach
The Desktop Media Player leverages modern development tools to deliver an efficient and polished experience:
- **Languages**: The application is built with C++ for high performance and ImGui for its lightweight and responsive graphical interface.
- **Frameworks and Libraries**: The project employs FFmpeg for reliable video decoding and processing.
- **Tools**: Git and CMake streamline development and build processes. Regular code reviews and testing are integrated to ensure quality and maintainability.

The technical implementation focuses on modular design, allowing flexibility for future feature additions and performance optimizations.

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

## Risks, Team Information, and Timeline
### Risks and Mitigation
1. **Library Familiarity**: Some team members are unfamiliar with FFmpeg and ImGui.
    - **Mitigation**: Allocate time for learning resources and prototyping early in the project.
2. **Coordination Challenges**: With a large team, maintaining consistent coding standards may be difficult.
    - **Mitigation**: Use detailed style guides and conduct weekly code reviews.
3. **Time Constraints**: Balancing project work with coursework may lead to delays.
    - **Mitigation**: Set clear milestones and prioritize high-impact tasks.

## Features

### Major Features (MVP)
1. Play MP4 video files.
2. Pause and resume playback.
3. Fast-forward and rewind.
4. Close Caption.
5. Simple Video Edits (trim and stitch).
6. Custom Playback Speed. 
7. Subtitle Translation

### Stretch Goals
1. Cross Platform.
2. Software Hub for Video Sharing API's.

## Development Timeline

| **Week(s)** | **Focus Area**                                         |  
|------------|--------------------------------------------------------|  
| **Weeks 2–3** | Research and setup                                     |  
| **Weeks 4–6** | Implement MVP features and basic GUI                   |  
| **Weeks 7–8** | Test MVP and begin implementing stretch goals          |  
| **Week 9** | Finalize features, optimize performance, and prepare documentation |


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

## Conclusion
The Desktop Media Player project combines innovative features with a user-centric approach to create a streamlined, reliable video playback and editing tool. By prioritizing simplicity, accessibility, and performance, this project aims to fill the gap between complex professional tools and basic video players. With a strong focus on modularity and cross-platform capabilities, the player offers an adaptable solution for modern media consumption and creation.