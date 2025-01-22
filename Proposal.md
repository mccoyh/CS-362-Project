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

### Abstract
A lightweight desktop application for seamless video playback. The player includes essential playback functionality with a focus on simplicity and performance. 

### Goal
To create a simple, efficient, and reliable video player for desktop platforms, offering smooth playback and a minimalist interface. 

### Current Practice
Existing video players are often overloaded with features, making them cumbersome for straightforward use cases. A variety of video players exist with extremely similar functionality that has barely evolved over the years. Most video players have been created for a single operating system, thus creating several UI's users have to learn to use multiple devices. 

### Novelty
This player prioritizes simplicity, lightweight design, and intuitive controls. Adding basic video editing will allow the average user to create simple videos without the need to learn complex programs with UI's that overload and confuse the user. Auto closecaptions and subtitle translation will increase user accessiblity. Our strech goals to create a cross platform and act as a hub for video sharing API's allows our video player to be used reguardless of the device.

### Effects
A successful implementation will provide users with a dependable, user-friendly video playback tool that simplifies basic video interactions.

### Technical Approach
- **Languages/Frameworks**: C++ with ImGui for the GUI
- **Libraries**: FFmpeg for video decoding
- **Tools**: Git, CMake

### Risks and Mitigation
1. **Learning Curve**: Unfamiliarity with libraries.
    - **Solution**: Allocate research and prototyping time early in the schedule.
2. **Team Coordination**: Coding inconsistencies.
    - **Solution**: Establish coding standards and conduct code reviews.
3. **Time Management**: Balancing workload with coursework.
    - **Solution**: Create and follow a detailed timeline with milestones.

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

## Non-functional Requirements

## External Requirements
    1. The product must be robust against errors, such as invalid video formats and other invalid user inputs. 
    2. The application must be able to download, install, and run.
    3. The software must build from source so that developers can add new video playing and editing features. 
    4. Each use case and main feature must not require more resources than available to be completed during the term. 
## Conclusion
This project delivers a streamlined desktop video player tailored for efficiency and usability. By maintaining a clear focus on core features and achievable goals, we aim to deliver a polished, functional product within the timeline.
