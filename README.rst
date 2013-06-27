Our myjfm_spider is a light weight configurable spider. So I want it to be a modularized software. Its architecture is illustrated in the doc/architecture.png
It mainly includes four modules, as follows:

1. **Downloader**: Each downloader is a thread. Its work contains:
        * get URLs from its own queue and,

        * assemble the HTTP requests and send them to the corresponding sites,

        * wait until the sites returning back the requested web pages,

        * put the web pages into '''the Global Page Snychronous Queue''',

        * go to the first step.

2. **Extractor**: Each extractor is also a thread. This module's responsibilities are as follows:

        * get web pages from '''the Global Page Snychronous Queue''' and, 

        * save these web pages on disk and then,

        * extract all the URLs from web pages,

        * put all the extracted URLs into '''the Global URL Snychronous Queue''',

        * go to the first step.

3. **Scheduler**: This is the core module of the spider. There also can have more than one. But, for now, one is enough. Its work contains:

        * get the URLs from '''the Global URL Snychronous Queue''',

        * check all the URLs one by one in this way: if the URL has been downloaded, then drop it, otherwise, check if this site has been downloaded within a given time interval. If true, put it in '''the Waiting Queue'''(This feature will be developped in the future version), otherwise, put this URL into the corresponding thread's queue(according to the (md5 value) modulo (numOfthreads)),

        * get all the time out URLs from '''the Waiting Queue'''(This feature will be developped in the future version), 

        * put them into the coresponding thread's queue(also according to the same algorithm as the previous step),

        * go to the first step.

4. **Main**: This module is the main thread of the process. When the program starts running, it configures the Downloaders, the Extractors and the Scheduler(s). Also, it can deal with the commands from the end user.
