class LBSoundManager {

	static AbstractWave m_SoundPlaying;

	static void OnRPC(ParamsReadContext ctx) {
		int type;
		if (!ctx.Read(type))
			return;
		vector position;
		string soundSet;
		switch (type) {
			case 1: {
				if (ctx.Read(position) && ctx.Read(soundSet))
					PlaySoundOnClient(position, soundSet);
				break;
			}
			case 2: {
				if (ctx.Read(soundSet))
					Play2DSound(soundSet);
				break;
			}
			case 3: {
				float duration;
				if (ctx.Read(position) && ctx.Read(soundSet) && ctx.Read(duration))
					PlaySoundLoopedOnClient(position, soundSet, null, duration);
				break;
			}
		}
	}

	static void PlaySoundLoopedOnClient(vector position, string soundSet, PlayerIdentity player = null, float maxDuration = -1) {
#ifdef SERVER
		ScriptRPC rpc = new ScriptRPC();
		int type = 3;
		rpc.Write(type);
		rpc.Write(position);
		rpc.Write(soundSet);
		rpc.Write(maxDuration);
		rpc.Send(null, LBmaster_Core_RPCs.PLAY_SOUND, true, player);
#else
		EffectSound sound = SEffectManager.PlaySound(soundSet, position, 0, 0, true);
		if (sound && maxDuration > 0.0)
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(StopSound, maxDuration * 1000, false, sound);
#endif
	}

	static void StopSound(EffectSound sound) {
		if (sound)
			sound.SoundStop();
	}

	static void PlaySoundOnClient(vector position, string soundSet, PlayerIdentity player = null) {
#ifdef SERVER
		ScriptRPC rpc = new ScriptRPC();
		int type = 1;
		rpc.Write(type);
		rpc.Write(position);
		rpc.Write(soundSet);
		rpc.Send(null, LBmaster_Core_RPCs.PLAY_SOUND, true, player);
#else
		SEffectManager.PlaySound(soundSet, position);
#endif
	}

	static void Play2DSound(string soundset, PlayerIdentity player = null) {
#ifdef SERVER
		ScriptRPC rpc = new ScriptRPC();
		int type = 2;
		rpc.Write(type);
		rpc.Write(soundset);
		rpc.Send(null, LBmaster_Core_RPCs.PLAY_SOUND, true, player);
#else
		SoundParams soundParams	= new SoundParams(soundset);
		if (soundParams.IsValid()) {
			SoundObjectBuilder soundBuilder	= new SoundObjectBuilder(soundParams);
			SoundObject soundObject	= soundBuilder.BuildSoundObject();
			soundObject.SetKind(WaveKind.WAVEEFFECT);

			m_SoundPlaying = g_Game.GetSoundScene().Play2D(soundObject, soundBuilder);
			if (m_SoundPlaying) {
				m_SoundPlaying.Loop(false);
				m_SoundPlaying.Play();
				LBLogger.Debug("Successfully played sound " + soundset, "Core");
			} else {
				LBLogger.Error("Could not play sound set " + soundset, "Core");
			}
		} else {
			LBLogger.Error("Could not find soundset " + soundset, "Core");
		}
#endif
	}

}