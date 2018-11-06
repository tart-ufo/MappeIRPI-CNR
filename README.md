# MappeIRPI-CNR
A GeoTiff processor that creates GIFs that represent Italian meteorological data and their correlation with registered landslides

The project consists in processing geoTiff images according to their color scale, adding the Italian background to them and an overlay of the warning zones. Then merge these frames into an animation, mp4 or gif.

The service is accessible from the specific site, and allows you to view the data of the last 24h, a specific day, and ask directly for the creation of customized files.

It is written in c ++, html and php.
It requires ffmpeg for joining frames, and imageMagick for create gifs.

More info on the wiki.
