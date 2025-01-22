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
| Nam Long Tran    | Developer |

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
1. Simple Video Edits  
    Actor: Product user  
    Goal: Edit a videos length and combine clips together.   
    Preconditions: The user opens the app and has a clip or clips to edit.  
    Postconditions: A new video clip containing the users edit.  
    List of steps:  

        a. User enters edit view and active video is imported.  
        b. User trims the video.  
        c. User uploads another clip.   
        d. User trims the second clip.  
        e. User joins the clips together.
        f. User saves the new clip.  

    Extensions: User cancels and exits editing clips.   
    Exceptions:  User is unable to upload an another clip. User is unable to save the video. User is unable to trim or stitch clips.

## Non-functional Requirements

## External Requirements
    1. The product must be robust against errors, such as invalid video formats and other invalid user inputs. 
    2. The application must be able to download, install, and run.
    3. The software must build from source so that developers can add new video playing and editing features. 
    4. Each use case and main feature must not require more resources than available to be completed during the term. 
## Conclusion
This project delivers a streamlined desktop video player tailored for efficiency and usability. By maintaining a clear focus on core features and achievable goals, we aim to deliver a polished, functional product within the timeline.
