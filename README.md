# Showoff-NVSE
An NVSE plugin which adds new functions, as well as some engine-level tweaks.\
For heathens who have forsaken the Vanilla-Style way. 

Documentation is available [here, on GeckWiki.com](https://geckwiki.com/index.php?title=Category:Functions_(ShowOff_NVSE)).

Releases are published [here, on Github](https://github.com/Demorome/Showoff-NVSE/releases).\
(Used to be on Nexus, recently changed)

If you wish to leave a comment, I (Demorome) will occasionally check the comment section [here, on Nexus](https://www.nexusmods.com/newvegas/mods/72541?tab=posts).\
You may also contact me directly on Discord. Username: `Demorome#2870`


# Requirements
Requires xNVSE v6.2.9 or greater: https://github.com/xNVSE/NVSE/releases.


# Installation
`ShowOffNVSE.dll` and `ShowOffNVSE.ini` must end up under `Fallout New Vegas/Data/nvse/plugins/..` (in the `plugins` folder).

Installing the primary .zip file from a mod manager should also work.
`ShowOffNVSE_INI.zip` need only be installed once; new INI options are automatically added to the file.


# Bug Fixes

    -Prevent ShowRaceMenu from resetting Ability and temporary Actor Effects.

# INI Options

## Subjective Bug Fixes
* ResetInterior Resets Actors

        The ResetInterior script function will now resurrect actors, as they would be if the cell was reset by waiting long enough.
        This renders it functionally identical to the ResetInteriorAlt function.
    
## Gameplay Tweaks
* Use Gamesettings For Fist Fatigue Damage

        Instead of multiplying the hit damage by 0.5 to get the fist fatigue damage, the formula is now:
		fistFatigueDmg = fHandFatigueDamageBase + (fHandFatigueDamageMult * regularDmg)
        
* Creatures Deal Melee Fatigue Damage

        Lets creatures deal fatigue damage from melee attacks. The formula is:
		meleeFatigueDmg = fHandFatigueDamageBase + (fHandFatigueDamageMult * regularDmg)
        
* Unarmed Weapons Deal Fatigue Damage

        Lets unarmed weapons deal fatigue damage, except for those with the 'Spin' animation type. The formula is:
		fatigueDmg = fFatigueAttackWeaponBase + (fFatigueAttackWeaponMult * regularDmg)
        
* No Self-Repairing Broken Items

        If enabled, broken items (0% condition) cannot be repaired in the pipboy repair menu. This encourages more attentive maintenance.
        
* No Vendor Repairing Broken Items

        If enabled, broken items (0% condition) cannot be repaired in the vendor repair menu. This encourages more attentive maintenance.


# Questions-&-Answers

This is a series of questions that generally get asked on pages for other NVSE plugins.

Q. Does this mod replace JIP LN NVSE Plugin, JohnnyGuitar NVSE, and SUP NVSE?\
A. No, ShowOff does not replace those plugins. It is meant to be installed and used alongside them.

Q. Will you support NVSE 5.1.4?\
A. No. Due to features in xNVSE that this plugins need, it is not feasible to support legacy versions of NVSE.

If you have anymore questions, feel free to join us on the [xNVSE community Discord server](https://discord.com/invite/EebN93s). 

# Permissions
All releases are free to be used by anyone.\
As for redistribution or taking code from this plugin for your own use...

Keep in mind that much of the ShowOff NVSE code is taken from others .\
I (Demorome) try to make it clear from where such code came from.\
If you want to use code I took from someone else, you should ask the original creator for permission.\
Besides that, you may use any of my code anywhere without asking for my permission, so long as you credit me.

# Credits
Kormakur, lStewieAl, c6, Luthien, TommInfinite, Jazzisparis, karut - for their invaluable help and the code I stole off all of them.

Code has been taken from the following plugins:
* [JohnnyGuitar NVSE](https://www.nexusmods.com/newvegas/mods/66927)
* [JIPLN NVSE](https://www.nexusmods.com/newvegas/mods/58277) 
* [lStewieAl's Tweaks](https://www.nexusmods.com/newvegas/mods/66347)
* [SUP NVSE](https://www.nexusmods.com/newvegas/mods/73160)

This plugin wouldn't be possible without everyone who contributed to (x)NVSE, and who made the example plugin.

* xNVSE and the example plugin are available here: https://github.com/xNVSE/NVSE
* The original NVSE is available here: http://www.nvse.silverlock.org/

AVeryUncreativeUsername / Trooper - for showing off his abilities,  coming up with the name, banner, and various functions. Also for documenting some of the functions.

anhatthezoo - For contributing the CreateFolder function.

CivisRomanus - For coming up with the the phrases which inspired this mod's name. 

Anro - motivating me to push this thing out.

Kormakur (again) - The single best thing to happen to New Vegas scripting.

External libraries used:
* [Armadillo](http://arma.sourceforge.net/docs.html)
* [SimpleINI](https://github.com/brofield/simpleini)
* [TaoJSON](https://github.com/taocpp/json)
* [TaoConfig](https://github.com/taocpp/config)

Code taken from:
* [Robert Penner's Easing Functions](http://robertpenner.com/easing/)

# See Also
[Crash Logger](https://www.nexusmods.com/newvegas/mods/72317) - if you're getting crashes with a DLL, this is the best thing to report issues to the author.

[Geck Extender](https://www.nexusmods.com/newvegas/mods/64888) - essential utility for anyone working with the GECK, many bugfixes and new features

[JIP LN NVSE](https://www.nexusmods.com/newvegas/mods/58277) - extends scripting capabilities far beyond limits of the vanilla game.

[Johnny Guitar NVSE](https://www.nexusmods.com/newvegas/mods/66927) - same as the above, but with more PLAY THE GUITAR, PLAY IT AGAIN, MY JOHNNYYYYYYYYYYYY.

[SUP NVSE](https://www.nexusmods.com/newvegas/mods/71878) - It's the same deal as the JIP and Johnny. More functions!

[Hot Reload](https://www.nexusmods.com/newvegas/mods/70962) - lets you use external editors for scripting and reload scripts without restarting the game, a must-have for any scripter.

[Improved Console](https://www.nexusmods.com/newvegas/mods/70801) - quite useful for quickly testing functions in console.

[lStewieAl's Tweaks](https://www.nexusmods.com/newvegas/mods/66347) - fixes numerous engine bugs and provides many configurable new features and tweaks to the game.

[Viva New Vegas](https://vivanewvegas.github.io/intro.html) - a great introductory guide for modding New Vegas which brought me back to this game.
