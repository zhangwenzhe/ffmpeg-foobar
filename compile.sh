#!/bin/bash
make
gcc -shared -Wl,-soname,libavfilter.so.7 -Wl,-Bsymbolic -Wl,--version-script,libavfilter/libavfilter.ver -Llibavcodec -Llibavdevice -Llibavfilter -Llibavformat -Llibavresample -Llibavutil -Llibpostproc -Llibswscale -Llibswresample -Wl,--as-needed -Wl,-z,noexecstack -Wl,--warn-common -Wl,-rpath-link=libpostproc:libswresample:libswscale:libavfilter:libavdevice:libavformat:libavcodec:libavutil:libavresample  -o libavfilter/libavfilter.so.7 libavfilter/aeval.o libavfilter/af_acontrast.o libavfilter/af_acopy.o libavfilter/af_acrossover.o libavfilter/af_acrusher.o libavfilter/af_adeclick.o libavfilter/af_adelay.o libavfilter/af_aderivative.o libavfilter/af_aecho.o libavfilter/af_aemphasis.o libavfilter/af_afade.o libavfilter/af_afftdn.o libavfilter/af_afftfilt.o libavfilter/af_afir.o libavfilter/af_aformat.o libavfilter/af_agate.o libavfilter/af_aiir.o libavfilter/af_alimiter.o libavfilter/af_amerge.o libavfilter/af_amix.o libavfilter/af_amultiply.o libavfilter/af_anequalizer.o libavfilter/af_anull.o libavfilter/af_apad.o libavfilter/af_aphaser.o libavfilter/af_apulsator.o libavfilter/af_aresample.o libavfilter/af_asetnsamples.o libavfilter/af_asetrate.o libavfilter/af_ashowinfo.o libavfilter/af_astats.o libavfilter/af_atempo.o libavfilter/af_biquads.o libavfilter/af_channelmap.o libavfilter/af_channelsplit.o libavfilter/af_chorus.o libavfilter/af_compand.o libavfilter/af_compensationdelay.o libavfilter/af_crossfeed.o libavfilter/af_crystalizer.o libavfilter/af_dcshift.o libavfilter/af_drmeter.o libavfilter/af_dynaudnorm.o libavfilter/af_earwax.o libavfilter/af_extrastereo.o libavfilter/af_firequalizer.o libavfilter/af_flanger.o libavfilter/af_haas.o libavfilter/af_hdcd.o libavfilter/af_headphone.o libavfilter/af_join.o libavfilter/af_loudnorm.o libavfilter/af_mcompand.o libavfilter/af_pan.o libavfilter/af_replaygain.o libavfilter/af_sidechaincompress.o libavfilter/af_silencedetect.o libavfilter/af_silenceremove.o libavfilter/af_stereotools.o libavfilter/af_stereowiden.o libavfilter/af_superequalizer.o libavfilter/af_surround.o libavfilter/af_tremolo.o libavfilter/af_vibrato.o libavfilter/af_volume.o libavfilter/af_volumedetect.o libavfilter/allfilters.o libavfilter/asink_anullsink.o libavfilter/asrc_anoisesrc.o libavfilter/asrc_anullsrc.o libavfilter/asrc_hilbert.o libavfilter/asrc_sinc.o libavfilter/asrc_sine.o libavfilter/audio.o libavfilter/avf_abitscope.o libavfilter/avf_ahistogram.o libavfilter/avf_aphasemeter.o libavfilter/avf_avectorscope.o libavfilter/avf_concat.o libavfilter/avf_showcqt.o libavfilter/avf_showfreqs.o libavfilter/avf_showspectrum.o libavfilter/avf_showvolume.o libavfilter/avf_showwaves.o libavfilter/avfilter.o libavfilter/avfiltergraph.o libavfilter/bbox.o libavfilter/buffersink.o libavfilter/buffersrc.o libavfilter/colorspace.o libavfilter/colorspacedsp.o libavfilter/dnn_backend_native.o libavfilter/dnn_interface.o libavfilter/drawutils.o libavfilter/ebur128.o libavfilter/f_bench.o libavfilter/f_cue.o libavfilter/f_drawgraph.o libavfilter/f_ebur128.o libavfilter/f_graphmonitor.o libavfilter/f_interleave.o libavfilter/f_loop.o libavfilter/f_metadata.o libavfilter/f_perms.o libavfilter/f_realtime.o libavfilter/f_reverse.o libavfilter/f_select.o libavfilter/f_sendcmd.o libavfilter/f_sidedata.o libavfilter/f_streamselect.o libavfilter/fifo.o libavfilter/formats.o libavfilter/framepool.o libavfilter/framequeue.o libavfilter/framesync.o libavfilter/generate_wave_table.o libavfilter/graphdump.o libavfilter/graphparser.o libavfilter/lavfutils.o libavfilter/log2_tab.o libavfilter/lswsutils.o libavfilter/motion_estimation.o libavfilter/pthread.o libavfilter/scale.o libavfilter/setpts.o libavfilter/settb.o libavfilter/split.o libavfilter/src_movie.o libavfilter/transform.o libavfilter/trim.o libavfilter/vaf_spectrumsynth.o libavfilter/vf_alphamerge.o libavfilter/vf_amplify.o libavfilter/vf_aspect.o libavfilter/vf_atadenoise.o libavfilter/vf_avgblur.o libavfilter/vf_bbox.o libavfilter/vf_bitplanenoise.o libavfilter/vf_blackdetect.o libavfilter/vf_blend.o libavfilter/vf_bm3d.o libavfilter/vf_bwdif.o libavfilter/vf_chromakey.o libavfilter/vf_ciescope.o libavfilter/vf_codecview.o libavfilter/vf_colorbalance.o libavfilter/vf_colorchannelmixer.o libavfilter/vf_colorconstancy.o libavfilter/vf_colorkey.o libavfilter/vf_colorlevels.o libavfilter/vf_colorspace.o libavfilter/vf_convolution.o libavfilter/vf_convolve.o libavfilter/vf_copy.o libavfilter/vf_crop.o libavfilter/vf_curves.o libavfilter/vf_datascope.o libavfilter/vf_dctdnoiz.o libavfilter/vf_deband.o libavfilter/vf_deblock.o libavfilter/vf_decimate.o libavfilter/vf_deflicker.o libavfilter/vf_dejudder.o libavfilter/vf_deshake.o libavfilter/vf_despill.o libavfilter/vf_detelecine.o libavfilter/vf_displace.o libavfilter/vf_drawbox.o libavfilter/vf_edgedetect.o libavfilter/vf_elbg.o libavfilter/vf_entropy.o libavfilter/vf_extractplanes.o libavfilter/vf_fade.o libavfilter/vf_fftdnoiz.o libavfilter/vf_fftfilt.o libavfilter/vf_field.o libavfilter/vf_fieldhint.o libavfilter/vf_fieldmatch.o libavfilter/vf_fieldorder.o libavfilter/vf_fillborders.o libavfilter/vf_floodfill.o libavfilter/vf_foobar.o libavfilter/vf_format.o libavfilter/vf_fps.o libavfilter/vf_framepack.o libavfilter/vf_framerate.o libavfilter/vf_framestep.o libavfilter/vf_gblur.o libavfilter/vf_gradfun.o libavfilter/vf_hflip.o libavfilter/vf_histogram.o libavfilter/vf_hqx.o libavfilter/vf_hue.o libavfilter/vf_hwdownload.o libavfilter/vf_hwmap.o libavfilter/vf_hwupload.o libavfilter/vf_hysteresis.o libavfilter/vf_idet.o libavfilter/vf_il.o libavfilter/vf_lenscorrection.o libavfilter/vf_limiter.o libavfilter/vf_lumakey.o libavfilter/vf_lut.o libavfilter/vf_lut2.o libavfilter/vf_lut3d.o libavfilter/vf_maskedclamp.o libavfilter/vf_maskedmerge.o libavfilter/vf_mergeplanes.o libavfilter/vf_mestimate.o libavfilter/vf_midequalizer.o libavfilter/vf_minterpolate.o libavfilter/vf_mix.o libavfilter/vf_neighbor.o libavfilter/vf_nlmeans.o libavfilter/vf_noise.o libavfilter/vf_normalize.o libavfilter/vf_null.o libavfilter/vf_overlay.o libavfilter/vf_pad.o libavfilter/vf_palettegen.o libavfilter/vf_paletteuse.o libavfilter/vf_pixdesctest.o libavfilter/vf_premultiply.o libavfilter/vf_pseudocolor.o libavfilter/vf_psnr.o libavfilter/vf_qp.o libavfilter/vf_random.o libavfilter/vf_readeia608.o libavfilter/vf_readvitc.o libavfilter/vf_remap.o libavfilter/vf_removegrain.o libavfilter/vf_removelogo.o libavfilter/vf_rotate.o libavfilter/vf_scale.o libavfilter/vf_selectivecolor.o libavfilter/vf_separatefields.o libavfilter/vf_setparams.o libavfilter/vf_showinfo.o libavfilter/vf_showpalette.o libavfilter/vf_shuffleframes.o libavfilter/vf_shuffleplanes.o libavfilter/vf_signalstats.o libavfilter/vf_sr.o libavfilter/vf_ssim.o libavfilter/vf_stack.o libavfilter/vf_swaprect.o libavfilter/vf_swapuv.o libavfilter/vf_telecine.o libavfilter/vf_threshold.o libavfilter/vf_thumbnail.o libavfilter/vf_tile.o libavfilter/vf_tonemap.o libavfilter/vf_transpose.o libavfilter/vf_unsharp.o libavfilter/vf_vectorscope.o libavfilter/vf_vflip.o libavfilter/vf_vfrdet.o libavfilter/vf_vibrance.o libavfilter/vf_vignette.o libavfilter/vf_vmafmotion.o libavfilter/vf_w3fdif.o libavfilter/vf_waveform.o libavfilter/vf_weave.o libavfilter/vf_xbr.o libavfilter/vf_yadif.o libavfilter/vf_zoompan.o libavfilter/video.o libavfilter/vsink_nullsink.o libavfilter/vsrc_cellauto.o libavfilter/vsrc_life.o libavfilter/vsrc_mandelbrot.o libavfilter/vsrc_testsrc.o libavfilter/x86/af_afir.o libavfilter/x86/af_afir_init.o libavfilter/x86/af_volume.o libavfilter/x86/af_volume_init.o libavfilter/x86/avf_showcqt.o libavfilter/x86/avf_showcqt_init.o libavfilter/x86/colorspacedsp.o libavfilter/x86/colorspacedsp_init.o libavfilter/x86/vf_blend.o libavfilter/x86/vf_blend_init.o libavfilter/x86/vf_bwdif.o libavfilter/x86/vf_bwdif_init.o libavfilter/x86/vf_framerate.o libavfilter/x86/vf_framerate_init.o libavfilter/x86/vf_gradfun.o libavfilter/x86/vf_gradfun_init.o libavfilter/x86/vf_hflip.o libavfilter/x86/vf_hflip_init.o libavfilter/x86/vf_idet.o libavfilter/x86/vf_idet_init.o libavfilter/x86/vf_limiter.o libavfilter/x86/vf_limiter_init.o libavfilter/x86/vf_maskedmerge.o libavfilter/x86/vf_maskedmerge_init.o libavfilter/x86/vf_noise.o libavfilter/x86/vf_overlay.o libavfilter/x86/vf_overlay_init.o libavfilter/x86/vf_psnr.o libavfilter/x86/vf_psnr_init.o libavfilter/x86/vf_removegrain_init.o libavfilter/x86/vf_ssim.o libavfilter/x86/vf_ssim_init.o libavfilter/x86/vf_threshold.o libavfilter/x86/vf_threshold_init.o libavfilter/x86/vf_w3fdif.o libavfilter/x86/vf_w3fdif_init.o libavfilter/x86/vf_yadif.o libavfilter/x86/vf_yadif_init.o libavfilter/x86/yadif-10.o libavfilter/x86/yadif-16.o libavfilter/yadif_common.o -lswscale -lavformat -lavcodec -lswresample -lavutil -pthread -lm -lm -lm -pthread -lm -lm -pthread -lm  -lmymatcher -lzffqueue
make
make install
