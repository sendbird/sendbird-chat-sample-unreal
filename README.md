
# Sendbird Chat Sample for Unreal

[![Platform](https://img.shields.io/badge/platform-unreal-black.svg)](https://github.com/sendbird/sendbird-chat-sample-unreal)
[![Languages](https://img.shields.io/badge/language-c++-black.svg)](https://github.com/sendbird/sendbird-chat-sample-unreal)

## Introduction

[Sendbird](https://sendbird.com) provides the chat API and SDK for your app, enabling real-time communication among the users.

- [**Chat Sample**](#chat-sample) has core chat features. Group channel and open channel are the two main channel types in which you can create various subtypes where users can send and receive messages. This sample is written with [Sendbird Chat SDK for Unreal](https://github.com/sendbird/sendbird-chat-sdk-unreal).

<br />

## 🔒 Security tip
When a new Sendbird application is created in the dashboard the default security settings are set permissive to simplify running samples and implementing your first code.

Before launching make sure to review the security tab under ⚙️ Settings -> Security, and set Access token permission to Read Only or Disabled so that unauthenticated users can not login as someone else. And review the Access Control lists. Most apps will want to disable "Allow retrieving user list" as that could expose usage numbers and other information.

## Requirements

- `UE_4.27`

## Supported Platforms

- `Android`, `iOS`, `Windows`, `Mac`

## Run the Sample

1. [Install **Unreal Engine** `4.27.x`](https://www.unrealengine.com/download).
2.  **Clone** the project from this repository.
    - `git clone git@github.com:sendbird/sendbird-chat-sample-unreal.git`
3. Use **Unreal Engine** to run `SendbirdSample.uproject` in sample root folder.
4. To replace Application ID with your own application ID, follow these steps. ([https://dashboard.sendbird.com](https://dashboard.sendbird.com))
    - Copy your application id from the [Sendbird Dashboard](https://dashboard.sendbird.com/).
    
    - In **Unreal Engine**, click `Open Level Blueprint`.

        ![Open Level Blueprint](https://i.imgur.com/drlv55C.png)

    - Replace Application ID with your Application ID.

        ![App Id](https://i.imgur.com/uvjLBNX.png)

5. Press the **Play** button to run the sample and test the features.
