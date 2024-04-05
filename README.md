# iLambda/PBCharacterMovement

This is a fork of [ProjectBorealis/PBCharacterMovement](https://github.com/ProjectBorealis/PBCharacterMovement), which aims to add features that were left out and/or not implemented at all in the original repo.
All of the added mechanics are similar to some of other Source Engine based games, like TF|2 and Apex Legends.

**IMPORTANT NOTE**: All of the code in this repository has been tested or is currently in testing, but bugs might remain. Be careful when using it in production. If you find a bug (or even better, find a fix for it), do not hesitate to create an issue.


A non-exhaustive list of such improvements:
* [Coyote time](https://kidscancode.org/godot_recipes/4.x/2d/coyote_time/index.html)
* Swimming
* Powersliding (based on Apex Legends sliding mechanics)
* Ladder climbing (based on Half Life 1 ladder physics)
* **[To be implemented]** Wallrunning (based on [Titanfall 2 wallrunning mechanics](https://youtu.be/n1mbGmOZWcU))
* **[To be implemented]** Mantling (idem)

The aforementioned mechanics are not supposed to be a 1:1 replica of the Source games they come from, but instead a good approximation of these movement mechanics, with basic movement tech being able to be replicated, such as:
* Ladder fall damage cancel
* Slide hopping
* Slide jumping
* ... and others

## Original repository
 
All basic information pertaining to walking, running, jumping, and crouching can be found on the [original repo](https://github.com/ProjectBorealis/PBCharacterMovement).
[Their blog](https://www.projectborealis.com/movement) also details some of the specifics of how the mechanics are implemented.
